'''
Alberto Garcia Fernandez

Sistemas Operativos - Practica 1 - Llamadas al sistema

Este programa verifica que el formato del entregable de la practica es el correcto (sigue las especificaciones de nombrado, y esta bien comprimido).

'''
import subprocess
import signal
import os
import glob
import time
import sys



def normalizeOutput(result):
	'''
	Funcion que normaliza una cadena de entrada con el fin de eliminar errores de formato. Reemplaza saltos de linea por tabuladores y elimina espacios repetidos y espacios al principio y final
	'''
	result = result.replace("\r\n","\n");
	result = result.replace("\r","\n");
	resultList = result.split("\n")
	resultNormalized = ""
	for resultLine in resultList:
		if resultLine == "":
			continue
		resultLine = resultLine.replace("\t", " ")
		resultLine = resultLine.replace("  ", " ")
		resultLine = resultLine.strip()
		resultNormalized += (resultLine + "\t")

	resultNormalized = resultNormalized.strip()

	return resultNormalized


def lookForTargetFile(folder,targetFile):
	'''
	Funcion que busca un fichero objetivo en una carpeta y devuelve su ruta
	'''
	toReturn="-"
	found=False
	subfolders=[x[0] for x in os.walk(folder)]
	for innerFolder in subfolders:
		folders=os.listdir(innerFolder)
		for item in folders:
			if item.lower()==targetFile.lower():
				return innerFolder+"/"+item
	return toReturn
	

def checkTargetFile(folder,targetFile):
	'''
	Funcion que comprueba que un fichero objetivo existe. Si no existe, lo intenta buscar en las carpetas hijas.
	'''
	toReturn="-"
	filePath=folder+targetFile
	if not os.path.exists(filePath):
		toReturn=lookForTargetFile(folder,targetFile)
	else:
		toReturn=filePath
	return toReturn


	os.chdir(cwd)

	#Obtenemos las salidas de la prueba con la que vamos a comparar y de la ejecucion del programa del alumno
	expectedResult = normalizeOutput(result1)
	obtainedResult = normalizeOutput(result2)

	#Comparamos las salidas e imprimimos resultados
	print ('CORRECTOR MYSIZE. Salida esperada:') 
	print(result1)
	print ('CORRECTOR MYSIZE. Salida del programa:')
	print(result2)

	if result1 == result2:
		print ('CORRECTOR MYSIZE. PRUEBA CORRECTA')
	else:
		print ('CORRECTOR MYSIZE. PRUEBA INCORRECTA. Las salidas difieren')




if(__name__=="__main__"):
	'''
	Funcion main de la aplicacion. Obtiene el fichero pasado como argumento. Lo descomprime, chequea su formato y finalmente lanza las pruebas.
	'''

	#Comprobamos que se ha pasado un fichero como argumento
	if not len(sys.argv) == 2:
		print ('Usage: python corrector.py <source code zip file>')
	else:
		print('CORRECTOR: checking', sys.argv[1])
		inputFile = sys.argv[1]
		
		#Comprobamos que el fichero existe
		if not os.path.isfile(inputFile):
			print("The file ", inputFile, " does not exist.")
			sys.exit(0)
	
		#Comprobamos el formato del nombre del fichero
		tokens=inputFile.replace(".zip","")
		tokens=tokens.split("_")
		if len(tokens) != 3 and len(tokens) != 4:
			print ('Incorrect name format of the file: ssoo_p1_AAAAA_BBBBB.zip')
			sys.exit(0)
			
		ssoo=tokens[0]
		p1=tokens[1]
		u1=tokens[2]
		u2=""
		if len(tokens)>3:
			u2=tokens[3]
		if not (ssoo == "ssoo" and p1 == "p1"):
			print ('Incorrect name format of the file: ssoo_p1_AAAAA_BBBBB.zip')
			sys.exit(0)

		print("CORRECTOR: NIA 1",u1, "NIA 2", u2)

		tempFolder="ssoo"


		if os.path.exists(tempFolder):
			subprocess.call(["rm", "-r",tempFolder])
		else:
			os.mkdir(tempFolder)	

		subprocess.call(["cp",inputFile,tempFolder])	
		#Descomprimimos el fichero en la carpeta temporal
		zipresult=subprocess.call(["unzip",tempFolder+"/*.zip","-d",tempFolder])
		if not zipresult == 0:
			print ('Error when decompressing the zip file.')
			sys.exit(0)

		''' 
		First file
		mycat.c
		'''
		#Definimos los nombres del fichero de codigo fuente, programa y fichero de prueba
		targetFile="mycat.c"
		programFile="mycat"
	
		#Buscamos el fichero fuente
		filePath=checkTargetFile(tempFolder,targetFile)
		if filePath=="-":
			print("CORRECTOR:", targetFile, "not found")
			subprocess.call(["rm", "-r",tempFolder])
			sys.exit(0)
	
		'''
                Second file
                myls.c
                '''

		#Definimos los nombres del fichero de codigo fuente y programa
		targetFile="myls.c"
		programFile="myls"

		#Buscamos el fichero fuente
		filePath=checkTargetFile(tempFolder,targetFile)
		if filePath=="-":
			print("CORRECTOR:", targetFile, "no encontrado")
			subprocess.call(["rm", "-r",tempFolder])
			sys.exit(0)

		'''
                Third file
                mysize.c
                '''

		#Definimos los nombres del fichero de codigo fuente y programa
		targetFile="mysize.c"
		programFile="mysize"

		#Buscamos el fichero fuente
		filePath=checkTargetFile(tempFolder,targetFile)
		if filePath=="-":
			print("CORRECTOR:", targetFile, "no encontrado")
			subprocess.call(["rm", "-r",tempFolder])
			sys.exit(0)


		subprocess.call(["rm", "-r",tempFolder])
		print("CORRECTOR: [CORRECT] Zip file is correct.")





