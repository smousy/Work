import argparse
import os
import Dicom_convector_jpg as Dcj
import File_reader
import Unet.check_prediction as check


def main(dicom_files):
    Output_Folder = 'Unet/data'
    template = 'masks'
    origin = 'imgs'
    SopInstanceUID_list = []
    if os.path.exists('logs.txt') is False:
        file = open('logs.txt', 'x')
        file.close()

    file_reader = File_reader.File_reader()
    start_number, Input_Image_List = file_reader.get_valid_path(dicom_files)
    if os.path.isdir(Output_Folder + '/' + template) is False:
        os.mkdir(Output_Folder + '/' + template)
    if os.path.isdir(Output_Folder + '/' + origin) is False:
        os.mkdir(Output_Folder + '/' + origin)

    template_path = Output_Folder + '/' + template
    origin_path = Output_Folder + '/' + origin

    for i in range(start_number, len(Input_Image_List)):
        convector = Dcj.Dicom_convector_jpg(Input_Image_List[i], template_path, origin_path)
        SopInstanceUID=convector.get_image(i)
        print(SopInstanceUID)
        SopInstanceUID_list.append(SopInstanceUID)
        create_number_file = open('create_number.txt', 'w')
        create_number_file.write(str(i))
        create_number_file.close()

    return SopInstanceUID_list

def get_args():
    parser = argparse.ArgumentParser(description='Train the UNet on images and target masks',
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument('-f', '--load', dest='load', type=str, default=['F:', 'D:/21.08.2020', 'D:/22.07.2020'],
                        help='Load model from a .pth file')

    return parser.parse_args()

if __name__ == '__main__':
    arg = get_args()
    directoris = arg.load.split(",")
    list = main(directoris)
    check.new_predict(list)
    os.remove("logs.txt")
    os.remove('readable_files.txt')
    os.remove('create_number.txt')
