rm -rf INSTDIR
./configure --prefix=`pwd`/INSTDIR --without-webkit1
make clean all
cd src ; strip fennekin.exe ; cd ..
make install

makensis fennekin.nsi
