rm -rf INSTDIR
./configure --prefix=`pwd`/INSTDIR --without-webkit1
make clean all install
cd src ; strip fennekin.exe ; cd ..
makensis fennekin.nsi
