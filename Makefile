all:
	make henkaku
	make menu
	make exploit
	make copy

exploit:
	cd payload; make; rm payload.bin.gz; gzip -9 -n -c payload.bin > payload.bin.gz; xxd -i payload.bin.gz > ../eboot/payload.h
	cd eboot; make

henkaku:
	cd taiHEN; mkdir build; cd build; cmake ..; make; cp taihen.skprx ../../bootstrap/res/taihen.skprx
	cd plugin; mkdir build; cd build; cmake ..; make; cp henkaku.skprx ../../bootstrap/res/henkaku.skprx; cp henkaku.suprx ../../bootstrap/res/henkaku.suprx

menu:
	cd bootstrap; mkdir build; cd build; cmake ..; make; xxd -i bootstrap.self > ../../payload/bootstrap.h

copy:
	cp eboot/PBOOT.PBP F:/pspemu/PSP/GAME/NPEZ00101/PBOOT.PBP

clean:
	-rm -rf taiHEN/build
	-rm -rf plugin/build
	-rm -rf bootstrap/build
	-rm bootstrap/res/taihen.skprx
	-rm bootstrap/res/henkaku.skprx
	-rm bootstrap/res/henkaku.suprx
	-rm payload/bootstrap.h
	-rm payload/payload.bin.gz
	-rm eboot/payload.h
	cd payload; make clean
	cd eboot; make clean
