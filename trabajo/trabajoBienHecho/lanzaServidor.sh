
# lanzaServidor.sh
# Lanza el servidor que es un daemon y varios clientes
# las ordenes est�n en un fichero que se pasa como tercer par�metro

#archivo="peticiones.log"

#Verificamos archivo existe
#if [ -f "$archivo" ]; then
#	rm "$archivo"
#	echo "Eliminado archivo: $archivo"
#fi 


./servidor &
./cliente nogal TCP ordenes.txt &
./cliente nogal TCP ordenes1.txt &
./cliente nogal TCP ordenes2.txt &
./cliente nogal UDP ordenes.txt &
./cliente nogal UDP ordenes1.txt &
./cliente nogal UDP ordenes2.txt &

 
