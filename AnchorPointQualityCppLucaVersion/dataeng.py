import sys

def get_folder():
    # Verifica se sono stati forniti argomenti da terminale
    if len(sys.argv) != 2:
        print("Usage: python programma.py <stringa>")
        return

    # Ottieni la stringa dall'argomento fornito
    nome_cartella = sys.argv[1]

    # ritorna il nome della cartella
    return nome_cartella
    
###################################################################################################
import numpy as np
import matplotlib.pyplot as plt
import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from rdkit import Chem
from rdkit.Chem import rdFingerprintGenerator
###################################################################################################

if __name__ == "__main__":
    raw_dataset        = get_folder()
    processed_dataset  = get_folder() + "_processed_data"
    
    # conteggio i sample
    samples = 0
    names   = []
    for root, dirnames, filenames in os.walk(raw_dataset):
            for dirname in dirnames:
                samples +=1
                names.append(dirname)
    
    print("There are %d samples in the folder" % samples)
    
    def extract_voxelization(filename):
        X = list()                            # coordinate atomiche
        
        file1 = open(filename, 'r')           # leggo il file
        Lines = file1.readlines()             # estraggo le linee
        
        A      = list()                       # coordinate anchor points
        sparse = list()                       # rappresentazione sparsa delle coordinate che descrivono la tasca
        
        x_min,y_min,z_min,x_max,y_max,z_max = 0,0,0,0,0,0
        
        for line in Lines:
            if line.startswith("ANCHOR"):
                A.append(np.array(line.split()[1:]).astype(float))
            if line.startswith("GRIDG"):
                data = np.array(line.split()[2:]).astype(float)
                x_min,y_min,z_min,x_max,y_max,z_max = data[0],data[1],data[2],data[3],data[4],data[5]
            if line.startswith("GRIDVX"):
                coords = np.array(line.split()[1:4]).astype(int)
                if line.split()[-2] == 'P':
                    sparse.append(coords)
        
        A      = np.array(A)
        sparse = np.array(sparse).astype(int)
        
        grid = np.zeros( (
            np.max(sparse[:,0])+1,
            np.max(sparse[:,1])+1,
            np.max(sparse[:,2])+1
            )
        )
        
        for s in sparse:
            grid[s[0],s[1],s[2]] = 1.
        
        specs = np.array([x_min,x_max,y_min,y_max,z_min,z_max])
        return grid, specs, A


    class AtomicStructure:
        def __init__(self, file_path):
            self.load_periodic_table()
            self.coords_list,self.atom_list = self.get_atom_coordinates_and_types(file_path)
            self.file_path   = file_path
            
        def get_atom_coordinates_and_types(self, filename):
            X = list()                            # coordinate atomiche
            A = list()                            # atom list

            file1 = open(filename, 'r')           # leggo il file
            Lines = file1.readlines()             # estraggo le linee

            reading_atoms = False
            for line in Lines:
                if line.startswith('@<TRIPOS>ATOM'):
                    reading_atoms = True
                else:
                    if reading_atoms:
                        if line.startswith("@<TRIPOS>BOND"):
                            break
                        else:
                            tokenized = line.split()
                            A.append(tokenized[5].split('.')[0])
                            X.append(tokenized[2:5])

            X = np.array(X).astype(float)
            A = np.array(A)

            return X,A
            
        def compute_center_of_mass(self):
            w = np.array([
                    self.atom_weight[atom_type]
                for atom_type in self.atom_list
            ])
            return (w[:,None] * self.coords_list).sum(axis = 0) / w.sum()
        
        def compute_finger_print(self):
            m   = Chem.rdmolfiles.MolFromMol2File(self.file_path)
            fpgen = rdFingerprintGenerator.GetMorganGenerator(radius=2)
            fp1 = fpgen.GetFingerprint(m)
            fp1 = np.array([bit for bit in fp1])
            return fp1
        
        def load_periodic_table(self):
            pt = pd.read_csv("periodic_table.csv").to_numpy()
            self.atom_weight = {}
            for atom_data in pt:
                atom_name                   = atom_data[2]
                atom_weight                 = atom_data[3]
                self.atom_weight[atom_name] = atom_weight
    
    print("caricamento dati")
    
    voxelization_specs  = []    # specifiche delle voxelizzazioni
    voxelizations       = []    # contiene le voxelizzazioni
    centers_of_mass     = []    # contiene i centri di massa
    finger_prints       = []    # contiene le fingerprints
    anchors             = []    # contiene la lista delle liste di anchor points

    count = 0
    N     = 300             # quanti sample caricare dal dataset?


    for root, dirnames, filenames in os.walk(raw_dataset):    
            for dirname in dirnames:
                #try:
                    if count > N:
                        break
                        
                    # trovo il path del ligando
                    ligand_name       = dirname + "/" + dirname+"_ligand.mol2"
                    whole_ligand_path = "./%s/%s" % (root,ligand_name)
                    
                    atomic_structure  = AtomicStructure(whole_ligand_path)
                    COM               = atomic_structure.compute_center_of_mass()
                    finger_print      = atomic_structure.compute_finger_print()
                    
                    
                    # trovo il path della tasca
                    pocket_name               = dirname + "/" + dirname+"_pocket.ligen.pdb"
                    whole_pocket_path         = './%s/%s' % (root,pocket_name)
                    vox,vox_specs,anchor      = extract_voxelization(whole_pocket_path)
                    
                    centers_of_mass.append(COM)
                    finger_prints.append(finger_print)
                    voxelizations.append(vox)
                    voxelization_specs.append(vox_specs)
                    anchors.append(anchor)
                    count += 1
                    
                    #if int((count)/(N/ 10)) != int( (count + 1) / (N/ 10) ) :
                    print(count)
    
    #######################################################################################################
    def create_folder_if_not_exists(folder_path):
        if not os.path.exists(folder_path):
            os.makedirs(folder_path)
            print(f"Folder '{folder_path}' created successfully.")
        else:
            print(f"Folder '{folder_path}' already exists.")
            
    #######################################################################################################
    
    create_folder_if_not_exists(processed_dataset)
    create_folder_if_not_exists("%s/voxelizations" % processed_dataset)
    create_folder_if_not_exists("%s/voxelization_specs" % processed_dataset)
    create_folder_if_not_exists("%s/centers_of_mass" % processed_dataset)
    create_folder_if_not_exists("%s/finger_prints" % processed_dataset)
    create_folder_if_not_exists("%s/anchors" % processed_dataset)

    
    
    for ( i, 
    voxelization,
    voxelization_spec,
    center_of_mass, 
    finger_print,
    anchor) in zip(
                                        range(len(voxelizations)),   # enumeratore
                                        voxelizations,               # voxelizzazioni
                                        voxelization_specs,          # specifiche delle voxelizzazioni
                                        centers_of_mass,             # centri di massa
                                        finger_prints,               # finger prints delle molecole
                                        anchors
                                        ):
        
        np.save("%s/voxelizations/voxelization_%d"   % (processed_dataset,i),  np.array(voxelization))
        np.save("%s/voxelization_specs/voxelization_spec_%d"   % (processed_dataset,i),  np.array(voxelization_spec))
        np.save("%s/centers_of_mass/center_of_mass_%d" % (processed_dataset,i),  np.array(center_of_mass))
        np.save("%s/finger_prints/finger_print_%d"          % (processed_dataset,i),  np.array(finger_print))
        np.save("%s/anchors/anchor_%d"          % (processed_dataset,i),  np.array(anchor))
        
    
    
    for i,name in zip(range(samples),names):
        print("### %s ###" % name)
        os.system("./main %s %d" % (processed_dataset,i))