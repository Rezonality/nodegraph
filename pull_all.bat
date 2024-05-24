git pull
cd libs/zing
git checkout main
git pull
cmd /c subs.bat
cd ../..
git pull
git submodule update --init --recursive

