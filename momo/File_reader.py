import os
from Filters import Graphic_annotation_filter, Bug_filter, Nipple_filter, Wrong_nipples_filter
import Validator


class File_reader:

    def __get_all_path__(self, Path, level=0):
        new_path = []
        returned = True
        start = 0

        if level > 0:
            start = 0

        # print(Path)
        for i in range(0, len(Path)):
            temp_path = os.listdir(Path[i])
            for j in range(start, len(temp_path)):
                if os.path.isdir(Path[i] + '/' + temp_path[j]) is True:
                    new_path.append(Path[i] + '/' + temp_path[j])
                else:
                    if os.path.getsize(Path[i] + '/' + temp_path[j]) > 20000:
                        new_path.append(Path[i] + '/' + temp_path[j])
                    returned = False
        if returned:
            return self.__get_all_path__(new_path, level + 1)
        else:
            return new_path



    def get_valid_path(self, directories):
        if os.path.exists('readable_files.txt') is True:
            file = open('readable_files.txt', 'r')
            new_path = []
            for line in file.readlines():
                new_path.append(line[:-1])

            if os.path.exists('create_number.txt') is True:
                file = open('create_number.txt', 'r')
                create_number = int(file.readline())
                return create_number, new_path

        else:
            Input_Folder = directories
            new_path = self.__get_all_path__(Input_Folder)
            #print(len(new_path))

            validator = Validator.Validator()
            #validator.add_filter(Graphic_annotation_filter.Graphic_annotation_filter())
            #validator.add_filter(Bug_filter.Bug_filter())
            #validator.add_filter(Nipple_filter.Nipple_filter())
            #validator.add_filter(Wrong_nipples_filter.Wrong_nipples_filter())

            new_path = validator.validate(new_path)
            #print(len(new_path))

            file = open('readable_files.txt', 'x')
            create_number_file = open('create_number.txt', 'x')
            create_number_file.write('0')
            create_number_file.close()
            for i in range(0, len(new_path)):
                file.write(new_path[i] + '\n')
            file.close()
            return 0, new_path
