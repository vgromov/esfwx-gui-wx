xgettext --force-po -C -T --no-location --no-wrap -d escore-gui -k_ -k_i -o locale/escore-gui.SRC.pot *.cpp *.h
msgcat -s --no-wrap --no-location -o locale/escore-gui.pot locale/escore-gui.SRC.pot locale/escore-gui.GUI.pot locale/escore-gui.MAN.pot