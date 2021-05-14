import os
from PIL import Image, ImageDraw
import math
from xml.dom import minidom

def barycenter(Image):
    X_sum = 0
    Y_sum = 0
    point_count = 0
    pix = Image.load()
    for i in range(0, Image.size[1]):
        for j in range(0, Image.size[0]):
            if pix[j, i] > 25:
                X_sum += j
                Y_sum += i
                point_count += 1

    if point_count != 0:
        return Y_sum / point_count, X_sum / point_count
    else:
        return "error"

def get_polyline(Image):
    pix = Image.load()
    points = []
    for i in range(0, Image.size[1]):
        for j in range(0, Image.size[0]):
            if pix[j, i] > 25:
                points.append((i, j))
                break

    for i in range(0, Image.size[1]):
        for j in range(0, Image.size[0]):
            if pix[Image.size[0]-j-1, Image.size[1]-i-1] > 25:
                points.append((Image.size[1]-i-1, Image.size[0]-j-1))
                break
    return points


def calculate_error():
    predict_mask_folder = "Unet/data/predicted_masks/"
    mask_folder = "Unet/data/masks/"

    predict_mask_points = []
    mask_points = []

    for i in os.listdir(predict_mask_folder):
        image1 = Image.open(predict_mask_folder + i)
        image2 = Image.open(mask_folder + i)

        result = barycenter(image1)
        if result != "error":
            predict_mask_points.append(result)
            mask_points.append(barycenter(image2))

    sum = 0
    for i in range(len(mask_points)):
        y = math.pow(mask_points[i][0] - predict_mask_points[i][0], 2)
        x = math.pow(mask_points[i][1] - predict_mask_points[i][1], 2)
        sum += math.sqrt(x + y)

    print(sum / len(mask_points))


def new_predict(list):
    doc = minidom.Document()
    print (list)

    for i in os.listdir("Unet/data/imgs/"):
        os.system("python Unet/predict.py -i Unet/data/imgs/" + i + " -o Unet/data/predicted_masks/" + i)

    predict_mask_folder = "Unet/data/predicted_masks/"

    log = open("logs.txt", "r")
    information_for_cords = []
    for line in log:
        line = line.replace('\n', '')
        line = line.split(" ")
        information_for_cords.append(line)

    info = None
    index = 0
    for i in os.listdir(predict_mask_folder):
        Image_node = doc.createElement('Image')
        Image_node.setAttribute("SopInstanceUID", list[index])
        Artefacts_node = doc.createElement('Artefacts')
        Image_node.appendChild(Artefacts_node)

        temp_Arctefact = doc.createElement('Artefact')
        temp_Arctefact.setAttribute("Id", "3")
        temp_Arctefact.setAttribute("Value", "30")

        temp_Property1 = doc.createElement('Property')
        temp_Property1.setAttribute("Id", "4")
        temp_Property1.setAttribute("Value", "40")

        temp_Property2 = doc.createElement('Property')
        temp_Property2.setAttribute("Id", "5")
        temp_Property2.setAttribute("Value", "50")

        temp_Property3 = doc.createElement('Property')
        temp_Property3.setAttribute("Id", "6")
        temp_Property3.setAttribute("Value", "61")

        temp_wkt_value = doc.createElement('WKT')
        temp_wkt_value.setAttribute("Value", "PolyLine 50,30,40,80")
        temp_Arctefact.appendChild(temp_Property1)
        temp_Arctefact.appendChild(temp_Property2)
        temp_Arctefact.appendChild(temp_Property3)
        temp_Arctefact.appendChild(temp_wkt_value)
        Artefacts_node.appendChild(temp_Arctefact)


        Artefact_node = doc.createElement('Artefact')
        Artefact_node.setAttribute("Id", "3")
        Artefact_node.setAttribute("Value", "393")


        WKTBorder_node = doc.createElement('WKTBorder')
        WKTCenter_node = doc.createElement('WKTCenter')
        WKTLine_node = doc.createElement('WKTLine')

        index += 1

        for j in information_for_cords:
            if j[0] == i:
                info = j

        image1 = Image.open(predict_mask_folder + i)
        result = barycenter(image1)
        if result != "error":
            end_line_point = 0.0
            if(int(info[4])<10):
                end_line_point = 1.0

            #print(result[0],result[1],image1.size[1],image1.size[0])
            Y = (result[0]*float(info[5])/1.917+int(info[1]))/(
                    image1.size[1]*float(info[5])/1.917+int(info[1])+int(info[2]))
            X = (result[1]*float(info[5])/1.917+int(info[3]))/(
                    image1.size[0]*float(info[5])/1.917+int(info[3])+int(info[4]))

            result = (X, Y)
            poly_line = get_polyline(image1)
            new_poly_line = []
            for j in poly_line:
                Y = (j[0] * float(info[5])/1.917 + int(info[1])) / (
                            image1.size[1]*float(info[5])/1.917 + int(info[1]) + int(info[2]))
                X = (j[1] * float(info[5])/1.917 + int(info[3])) / (
                            image1.size[0]*float(info[5])/1.917 + int(info[3]) + int(info[4]))

                new_poly_line.append((X, Y))

            WKTCenter_node.setAttribute("Value","POINT ("+ str(result[0])+" "+str(result[1])+")" )
            line = "POLYGON (( "
            for j in new_poly_line:
                line += (str(j[0])+" "+str(j[1])+", ")
            line = line[:-2]
            line += "))"

            WKTBorder_node.setAttribute("Value", line)
            WKTLine_node.setAttribute("Value","LINESTRING("+ str(result[0])+" "+str(result[1])+", "+str(end_line_point)+" "+str(result[1])+")")
            Artefact_node.appendChild(WKTBorder_node)
            Artefact_node.appendChild(WKTCenter_node)
            Artefact_node.appendChild(WKTLine_node)
            Artefacts_node.appendChild(Artefact_node)
        doc.appendChild(Image_node)
    xml_str = doc.toprettyxml(indent="  ", encoding='UTF-8')

    result_file = open("D:/Work/test grpc/grpc/clientOstis/barry_center_poly_line.xml", "w")
    result_file.write(str(xml_str, 'UTF-8'))
    result_file.close()

    result_file = open("D:/Work/test grpc/grpc/server/barry_center_poly_line.xml", "w")
    result_file.write(str(xml_str, 'UTF-8'))
    result_file.close()
