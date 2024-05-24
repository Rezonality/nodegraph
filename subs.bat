git pull
git submodule update --init --recursive
cd libs/zing
git checkout main
cmd /c subs.bat
cd ../..
