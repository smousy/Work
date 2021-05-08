from Filters import Filter


class Bug_filter(Filter.Filter):
    def validate(self,DCM_Img):
        flag = True
        graphic_annotation = DCM_Img.get(0x00700001)[0]
        text_objects = graphic_annotation.get(0x00700008)
        graphic_objects = graphic_annotation.get(0x00700009)
        for j in range(0, len(text_objects.value)):
            if text_objects[j].get(0x00700295).value != graphic_objects[j].get(0x00700295).value:
                flag = False
                print("cords_bug")

        return flag