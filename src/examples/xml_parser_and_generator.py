import xml.etree.ElementTree as ET
import os
import shutil

def main():
    path_XML = "/home/rolandb/2324_INFOB318_KILOBOT/src/argos3-kilobot/src/examples/experiments/kilobot_aggregation.argos"
    path_controlor = "/home/rolandb/2324_INFOB318_KILOBOT/src/argos3-kilobot/src/examples/behaviors"

    # Lire ficher XML et extraire le nombre de team
    tree = ET.parse(path_XML)
    root = tree.getroot()
    
    for controller in root.findall('controllers'):
        nb_team = controller.find('kilobot_controller').find('params').get('nb_team')
        print(nb_team)

 
    # Checker s'il existe déja nb_team fichier c
    for i in range(2,int(nb_team)+1):
        
        new_path = path_controlor + "/aggregation" + str(i) + ".c"
        
        if not os.path.exists(new_path):
            # création du fichier c supplémentaire
            shutil.copyfile(path_controlor + "/aggregation.c",new_path)
            
            fichierC = open(new_path,'r+')
            
            new_value = "kilo_team = " + str(i)
            data = fichierC.read()
            new_data = data.replace("kilo_team = 1", new_value)
            fichierC.seek(0)
            fichierC.write(new_data)
            
            fichierC.close()


if __name__ == "__main__":
    main()