import pydicom
from Filters import Filter


class Graphic_annotation_filter(Filter.Filter):
    def validate(self, DCM_Img):
        flag = True
        if DCM_Img.get(0x00700001) is None:
            flag = False
            print("No graphic_annotation")
        return flag