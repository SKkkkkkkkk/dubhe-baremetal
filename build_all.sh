cd app/a55/cli/;
rm -rf build;
./build.sh;

cd - ;
rm -rf build;
cd app/a55/dw_apb_ssi;
./build.sh;

cd - ;
rm -rf build ;
cd app/a55/dw_apb_ssi;
./build.sh;

cd - ;
rm -rf build ;
cd app/a55/dw_apb_timer;
./build.sh;

cd - ;
rm -rf build ;
cd app/a55/dw_mbox_a2b;
./build.sh;

cd - ;
rm -rf build ;
cd app/a55/freertos_basic;
./build.sh;

cd - ;
rm -rf build  ;
cd app/a55/fwu_dram;
./build.sh;

cd - ;
rm -rf build ;
cd app/a55/fwu_sram;
./build.sh;

cd - ;
rm -rf build ;
cd app/a55/generic_timers;
./build.sh;

cd - ;
rm -rf build ;
cd app/a55/gpio;
./build.sh;

cd - ;
rm -rf build ;
cd app/a55/hello;
./build.sh;

cd - ;
rm -rf build ;
cd app/a55/i2c;
./build.sh;

cd - ;
rm -rf build ;
cd app/cv32e40p/hello;
./build.sh;

cd - ;
rm -rf build ;
cd app/m3/cli;
./build.sh;

cd - ;
rm -rf build ;
cd app/m3/dw_apb_timer;
./build.sh;

cd - ;
rm -rf build ;
cd app/m3/freertos_basic;
./build.sh;

cd -;
rm -rf build;
cd app/m3/pm_demo;
./build.sh;

cd -;
rm -rf build;
cd app/m3/rom2xxx;
./build.sh;
