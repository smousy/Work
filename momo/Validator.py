import pydicom

class Validator:
    filter_list = []

    def add_filter(self, Filter):
        self.filter_list.append(Filter)

    def validate(self, Path_list):
        index = 0

        while index < len(Path_list):
            DCM_Img = pydicom.read_file(Path_list[index])

            #print(index)
            flag = True
            for i in self.filter_list:
                flag = i.validate(DCM_Img)
                if not flag:
                    del(Path_list[index])
                    break
            if flag:
                index += 1
        return Path_list
