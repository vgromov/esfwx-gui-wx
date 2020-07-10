rem esfwx-gui documentation generation

set start="%cd%"

for %%d in (escore-gui escomm-gui) do (
	cd %%d
  doxygen Doxyfile
  cd %start%
)

cd %start%\doc
for %%d in (escore-gui escomm-gui) do (
cd %%d
make.bat
copy /Y refman.pdf ..\%%d.pdf
cd %start%\doc
)

cd %start%