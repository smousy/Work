import cv2
import pydicom
import numpy
import os
from multiprocessing import Process
from multiprocessing import shared_memory
from multiprocessing import Value


class Object_data:
    object_id = 0
    name = ''
    graphic_data = []
    graphic_type = ''


class Dicom_convector_jpg:
    Rescale_Slope = 0.0
    Rescale_Intercept = 0.0
    Window_Max = 0.0
    Window_Min = 0.0

    Instance_Number = 0.0
    rows = 0
    cols = 0
    template_folder = ''
    origin_folder = ''
    Path = ''

    def __init__(self, Path, template_folder, origin_folder):
        self.origin_folder = origin_folder
        self.template_folder = template_folder
        self.Path = Path

    def __get_items__(self, DCM_Img):
        graphic_annotation = DCM_Img.get(0x00700001)[0]
        text_objects = graphic_annotation.get(0x00700008)
        graphic_objects = graphic_annotation.get(0x00700009)
        object_list = []

        for i in range(0, len(text_objects.value)):
            item = Object_data()
            item.name = text_objects[i].get(0x00700006).value[:-2]
            item.object_id = text_objects[i].get(0x00700295).value
            for j in range(0, len(graphic_objects.value)):
                if graphic_objects[j].get(0x00700295).value == item.object_id:
                    item.graphic_data = graphic_objects[j].get(0x00700022).value
                    item.graphic_type = graphic_objects[j].get(0x00700023).value
                    break
            object_list.append(item)

        for i in range(0, len(object_list)):
            for j in range(0, len(object_list[i].graphic_data)):
                if j % 2 == 1:
                    object_list[i].graphic_data[j] = round(object_list[i].graphic_data[j]*self.rows)
                else:
                    object_list[i].graphic_data[j] = round(object_list[i].graphic_data[j]*self.cols)
        return object_list

    def __draw_lines__(self, object_list):
        template = numpy.zeros((self.rows, self.cols), numpy.uint8)
        #template = cv2.cvtColor(template, cv2.COLOR_GRAY2BGR)
        for i in range(0, len(object_list)):
            if object_list[i].graphic_type == 'ELLIPSE':
                data = object_list[i].graphic_data

                xCenter = data[4]
                yCenter = data[1]
                axes = (round((data[2]-data[0])/2), round((data[7]-data[5])/2))

                if object_list[i].name == '<тип=сосок/>':
                    cv2.ellipse(template, (xCenter, yCenter), axes, 0, 0, 360, (255, 255, 255), -1)

            elif object_list[i].graphic_type == 'POLYLINE':
                points = []
                size = len(object_list[i].graphic_data)
                data = object_list[i].graphic_data

                for j in range(0, size-2, 2):
                    points.append([data[j], data[j+1]])
                points.append([data[size-2], data[size-1]])
                if object_list[i].name == '<тип=сосок/>':
                    cv2.fillConvexPoly(template, numpy.array(points, 'int32'), (255, 255, 255))

        return template

    def __read_thread__(self, rows_start, rows_finish, Pixels, memory_name):
        buffer = shared_memory.SharedMemory(name=memory_name)
        New_Img = numpy.ndarray((self.rows, self.cols), numpy.uint8, buffer=buffer.buf)
        for i in range(rows_start, rows_finish):
            for j in range(0, self.cols):
                Pix_Val = Pixels[i][j]
                Rescale_Pix_Val = Pix_Val * self.Rescale_Slope + self.Rescale_Intercept

                if (Rescale_Pix_Val > self.Window_Max):  # if intensity is greater than max window
                    New_Img[i][j] = 255
                elif (Rescale_Pix_Val < self.Window_Min):  # if intensity is less than min window
                    New_Img[i][j] = 0
                else:
                    New_Img[i][j] = int(((Rescale_Pix_Val - self.Window_Min) / (self.Window_Max - self.Window_Min)) * 255)  # Normalize the intensities
        New_Img[0][0] = 0
        New_Img[self.rows - 1][0] = 0
        New_Img[0][self.cols - 1] = 0
        New_Img[self.rows - 1][self.cols - 1] = 0
        buffer.close()

    def __left__(self, img, ret_value):
        flag = True
        left_counter = 0

        for i in range(0, self.cols):
            for j in range(0, self.rows):
                if img[j][i] != 0 and img[j][i] != 1:
                    flag = False
                    break
            if flag:
                left_counter += 1
            else:
                break
        ret_value.value = left_counter

    def __right__(self, img, ret_value):
        flag = True
        right_counter = 0

        for i in range(0, self.cols):
            for j in range(0, self.rows):
                if img[j][self.cols - i - 1] != 0 and img[j][self.cols - i - 1] != 1:
                    flag = False
                    break
            if flag:
                right_counter += 1
            else:
                break
        ret_value.value = right_counter

    def __top__(self, img, ret_value):
        flag = True
        top_counter = 0

        for i in range(0, self.rows):
            for j in range(0, self.cols):
                if img[i][j] != 0 and img[i][j] != 1:
                    flag = False
                    break
            if flag:
                top_counter += 1
            else:
                break
        ret_value.value = top_counter

    def __bottom__(self, img, ret_value):
        flag = True
        bottom_counter = 0

        for i in range(0, self.rows):
            for j in range(0, self.cols):
                if img[self.rows - i - 1][j] != 0 and img[self.rows - i - 1][j] != 1:
                    flag = False
                    break
            if flag:
                bottom_counter += 1
            else:
                break
        ret_value.value = bottom_counter

    def __cut__(self, img):
        left_counter = Value("i", 0, lock=False)
        right_counter = Value("i", 0, lock=False)
        top_counter = Value("i", 0, lock=False)
        bottom_counter = Value("i", 0, lock=False)

        left_process = Process(target=self.__left__, args=(img, left_counter))
        right_process = Process(target=self.__right__, args=(img, right_counter))
        top_process = Process(target=self.__top__, args=(img, top_counter))
        bottom_process = Process(target=self.__bottom__, args=(img, bottom_counter))

        left_process.start()
        right_process.start()
        top_process.start()
        bottom_process.start()

        left_process.join()
        right_process.join()
        top_process.join()
        bottom_process.join()

        if left_counter.value >= 25:
            left_counter.value -= 25

        if right_counter.value >= 25:
            right_counter.value -= 25

        if top_counter.value >= 25:
            top_counter.value -= 25

        if bottom_counter.value >= 25:
            bottom_counter.value -= 25
        return left_counter.value, right_counter.value, top_counter.value, bottom_counter.value

    def get_image(self, number):
        DCM_Img = pydicom.read_file(self.Path)

        self.rows = DCM_Img.get(0x00280010).value  # Get number of rows from tag (0028, 0010)
        self.cols = DCM_Img.get(0x00280011).value  # Get number of cols from tag (0028, 0011)

        self.Instance_Number = int(DCM_Img.get(0x00200013).value)  # Get actual slice instance number from tag (0020, 0013)

        Window_Center = int(DCM_Img.get(0x00281050).value)  # Get window center from tag (0028, 1050)
        Window_Width = int(DCM_Img.get(0x00281051).value)  # Get window width from tag (0028, 1051)

        self.Window_Max = int(Window_Center + Window_Width / 2)
        self.Window_Min = int(Window_Center - Window_Width / 2)

        if DCM_Img.get(0x00281052) is None:
            self.Rescale_Intercept = 0
        else:
            self.Rescale_Intercept = int(DCM_Img.get(0x00281052).value)

        if DCM_Img.get(0x00281053) is None:
            self.Rescale_Slope = 1
        else:
            self.Rescale_Slope = int(DCM_Img.get(0x00281053).value)

        origin = numpy.zeros((self.rows, self.cols), numpy.uint8)
        Pixels = DCM_Img.pixel_array
        #object_list = self.__get_items__(DCM_Img)

        cpu_number = os.cpu_count()-1
        if self.rows % cpu_number != 0:
            rows_number = int((self.rows - (self.rows % cpu_number)) / cpu_number)
        else:
            rows_number = int(self.rows / cpu_number)

        process = []
        start = 0
        finish = rows_number

        shared_pixels = shared_memory.SharedMemory(create=True, size=origin.nbytes)
        New_Img = numpy.ndarray(origin.shape, dtype=origin.dtype, buffer=shared_pixels.buf)
        New_Img[:] = origin[:]
        memory_name = shared_pixels.name

        for i in range(0, cpu_number-1):
            process.append(Process(target=self.__read_thread__, args=(start, finish, Pixels, memory_name)))
            start += rows_number
            finish += rows_number
            process[i].start()

        process.append(Process(target=self.__read_thread__, args=(start, finish+(self.rows % cpu_number), Pixels, memory_name)))
        process[cpu_number-1].start()

        for i in range(0, cpu_number):
            process[i].join()

        left_counter, right_counter, top_counter, bottom_counter = self.__cut__(New_Img)

        size_coff = (self.cols - left_counter - right_counter)/512
        New_Img = cv2.cvtColor(New_Img, cv2.COLOR_GRAY2BGR)
        New_Img = New_Img[top_counter:self.rows-bottom_counter, left_counter:self.cols-right_counter]
        new_size = (int(New_Img.shape[1]/size_coff), int(New_Img.shape[0]/size_coff))
        New_Img = cv2.resize(New_Img, new_size, interpolation=cv2.INTER_AREA)
        cv2.imwrite(self.origin_folder + '/' + str(number) + '.jpg', New_Img)

        #template = self.__draw_lines__(object_list)
        #template = template[top_counter:self.rows-bottom_counter, left_counter:self.cols-right_counter]
        #template_new_size = (int(template.shape[1]/size_coff), int(template.shape[0]/size_coff))
        #template = cv2.resize(template, template_new_size, interpolation=cv2.INTER_AREA)
        #cv2.imwrite(self.template_folder + '/' + str(number) + '.jpg', template)

        log = open('logs.txt', 'a')
        log.write(str(number)+'.jpg'+" "+str(top_counter)+" " + str(bottom_counter)+" "+str(left_counter)+" " + str(right_counter)+" "+str(size_coff)+"\n")
        log.close()
        shared_pixels.close()
        shared_pixels.unlink()
        return DCM_Img.get(0x00080018).value
