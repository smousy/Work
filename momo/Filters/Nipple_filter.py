import pydicom
from Filters import Filter

class Nipple_filter(Filter.Filter):
    def validate(self, DCM_Img):
        graphic_annotation = DCM_Img.get(0x00700001)[0]
        text_objects = graphic_annotation.get(0x00700008)

        flag = False
        for j in range(0, len(text_objects.value)):
            name = text_objects[j].get(0x00700006).value[:-2]
            if name == '<тип=сосок/>':
                flag = True
        if not flag:
            print("No nipple markings")
        return flag