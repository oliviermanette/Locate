root = '14_23_24';

for side=1:2
    if (side == 2)
        root = [root(1:3) ,strrep(root(4:end),'_','R')];
    end
    filename = [root,'.DAT']
    fileID = fopen(filename);
    B = fread(fileID,Inf,'int16',0,'b');
    fclose(fileID);
    Fs = round(size(B,1)/(60*0.5))
    spectrogram(B,1000,300,1000,Fs,'yaxis')
    saveas(gcf,[root,'.png'])
    C = B/max(abs(B));
    audiowrite([root,'.wav'],C,Fs);
    DSRate = 2; % Downsampling level
    k = 1;
    if (side == 1)
        D_L = zeros(floor(size(B,1)/DSRate),1);
        D_R = zeros(floor(size(B,1)/DSRate),1);
    elseif (side == 2)
        D_R = zeros(floor(size(B,1)/DSRate),1);
    end
    goto = size(B,1);
    for j=1:DSRate:goto % Downsampling 
        if (side == 2)
            D_R(k) = mean(B(j:j+DSRate-1));
        else
            D_L(k) = mean(B(j:j+DSRate-1));
        end
        k = k +1;
        if ((k>size(D_L,1))||(k>size(D_R,1)))
            break
        end
    end
    if (side == 2)
        Fs = round(size(D_R,1)/(60*0.5))
        spectrogram(D_R,1000,300,1000,Fs,'yaxis')
        saveas(gcf,[root,'fs20000.png'])
        C = D_R/max(abs(D_R));
    else
        Fs = round(size(D_L,1)/(60*0.5))
        spectrogram(D_L,1000,300,1000,Fs,'yaxis')
        saveas(gcf,[root,'fs20000.png'])
        C = D_L/max(abs(D_R));
    end
    audiowrite([root,'fs20000.wav'],C,Fs);
end

