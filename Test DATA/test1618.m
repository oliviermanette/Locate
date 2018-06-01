root = '14_22R54';
filename = [root,'.DAT']
fileID = fopen(filename);
B = fread(fileID,Inf,'int16',0,'b');
fclose(fileID);
Fs = round(size(B,1)/(60*0.5))
spectrogram(B,1000,300,1000,Fs,'yaxis')
saveas(gcf,[root,'.png'])
C = B/max(abs(B));
audiowrite([root,'.wav'],C,Fs);
DSRate = 2;
k = 1;
D_R = zeros(floor(size(B,1)/DSRate),1);
goto = size(B,1);
for j=1:DSRate:goto
D_R(k) = mean(B(j:j+DSRate-1));
k = k +1;
if k>size(D_R,1)
break
end
end
Fs = round(size(D_R,1)/(60*0.5))
spectrogram(D_R,1000,300,1000,Fs,'yaxis')
saveas(gcf,[root,'fs20000.png'])
C = D_R/max(abs(D_R));
audiowrite([root,'fs20000.wav'],C,Fs);