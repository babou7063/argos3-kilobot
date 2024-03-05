import xml.etree.ElementTree as ET
import os
import shutil


def main():
    parser_path = os.path.dirname(os.path.abspath(__file__))
    path_XML = parser_path + "/experiments/kilobot_aggregation.argos"
    path_controller = parser_path + "/behaviors"

    # Read XML file and extract team numbers
    tree = ET.parse(path_XML)
    root = tree.getroot()
    
    for controller in root.findall('controllers'):
        nb_team = controller.find('kilobot_controller').find('params').get('nb_team')

 
    # Checks if a .c file already exists
    for i in range(2,int(nb_team)+1):
        
        new_path = path_controller + "/aggregation" + str(i) + ".c"
        
        if not os.path.exists(new_path):
            # Creating an additional .c file
            shutil.copyfile(path_controller + "/aggregation.c",new_path)
            
            fichierC = open(new_path,'r+')
            
            new_value = "KILO_TEAM " + str(i)
            data = fichierC.read()
            new_data = data.replace("KILO_TEAM 1", new_value)
            fichierC.seek(0)
            fichierC.write(new_data)
            
            fichierC.close()

if __name__ == "__main__":
    main()