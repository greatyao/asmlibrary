echo ## Static image aligment ##
fit.exe -m my68-1d.amf -h haarcascade_frontalface_alt2.xml -i multipeople.jpg 
fitD.exe -m my68-1d.amf -h haarcascade_frontalface_alt2.xml -i multipeople2.jpg -n 30

echo ## face tracking from video ##
fit.exe -m my68-1d.amf -h haarcascade_frontalface_alt2.xml -v seq1.avi -n 24

echo ## face tracking from live camera ##
fit.exe -m my68-1d.amf -h haarcascade_frontalface_alt2.xml -c 0 -n 24

pause