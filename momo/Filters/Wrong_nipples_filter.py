import pydicom
from Filters import Filter


class Wrong_nipples_filter(Filter.Filter):
    def validate(self, DCM_Img):
        flag = False
        graphic_annotation = DCM_Img.get(0x00700001)[0]
        text_objects = graphic_annotation.get(0x00700008)
        graphic_objects = graphic_annotation.get(0x00700009)

        graphic_data = []
        for i in range(0, len(text_objects.value)):
            if text_objects[i].get(0x00700006).value[:-2] == '<тип=сосок/>':
                object_id = text_objects[i].get(0x00700295).value
                for j in range(0, len(graphic_objects.value)):
                    if graphic_objects[j].get(0x00700295).value == object_id:
                        graphic_data = graphic_objects[j].get(0x00700022).value
                        break

        rows = DCM_Img.get(0x00280010).value
        cols = DCM_Img.get(0x00280011).value

        for j in range(0, len(graphic_data)):
            if j % 2 == 1:
                graphic_data[j] = round(graphic_data[j] * rows)
            else:
                graphic_data[j] = round(graphic_data[j] * cols)

        maxY = graphic_data[1]
        minY = graphic_data[1]
        for i in range(1, len(graphic_data), 2):
            if maxY < graphic_data[i]:
                maxY = graphic_data[i]
            if minY > graphic_data[i]:
                minY = graphic_data[i]

        average = round((maxY+minY)/2)
        Pixels = DCM_Img.pixel_array
        border = 0

        for i in range(0, len(Pixels[average])):
            border = i
            if Pixels[average][i] > 2:
                break

        if border < 10:
            for i in range(0, len(Pixels[average])):
                border = len(Pixels[average]) - i - 1
                if Pixels[average][len(Pixels[average]) - i - 1] > 2:
                    border -= 10
                    for j in range(0, len(graphic_data), 2):
                        if graphic_data[j] > border:
                            flag = True
                            break
                    break
        else:
            border += 10
            for i in range(0, len(graphic_data), 2):
                if graphic_data[i] < border:
                    flag = True
                    break

        if not flag:
            print("Nipple in wrong place")
        return flag