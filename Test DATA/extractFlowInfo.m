%% extractFlowInfo.m
% Le programme MATLAB doit prendre le flux D_L et D_R et d�tecter les informations 
% suivantes : 
% 
% - dur�e dureeL = finL-debutL 
% 
% - position du max relativement au d�but [valeur, indiceL] = max(abs(D_L)); 
% poxMaxL = indiceL - debutL; 
% 
% - valeur moyenne mean(D_L) 
% 
% - amplitude max absolue ampLa = max(abs(D_L)) 
% 
% - amplitude max ampL = max(D_L)-min(D_L) 
% 
% - puissance absolue puissanceL = sum(abs(D_L))
% 
% - ecart de timing : debutL-debutR
% 
% - ecart de timing des max : posMaxL - posMaxR * Pas tout � fait faut prendre 
% la m�me r�f�rence qui serait le onset du premier � se manifester
% 
% - �cart relatif de dur�e : dureeL/dureeR
% 
% - �cart relatif d'amplitude : ampLa/ampRa et ampL/ampR
% 
% - �cart relatif de puissance : puissanceL/puissanceR
%% On commence par lire les fichiers bruts *.DAT et extraire les fr�quences
root = '14_26_25';
DSRate = 2; % Downsampling level

for side=1:2
    if (side == 2)
        root = [root(1:3) ,strrep(root(4:end),'_','R')];
    end
    filename = [root,'.DAT']
    fileID = fopen(filename);
    B = fread(fileID,Inf,'int16',0,'b');
    fclose(fileID);
    Fs = round(size(B,1)/(60*0.5));
    
    C = B/max(abs(B));
    
    k = 1;
    tpsTotal = floor(size(B,1)/DSRate);
    if (side<2)
        sensors = zeros(tpsTotal,2);
    end
    FsSensors = Fs/DSRate;
    goto = size(B,1);
    for j=1:DSRate:goto % Downsampling 
        sensors(k,side) = mean(B(j:j+DSRate-1));
        k = k +1;
        if (k>tpsTotal)
            break
        end
    end
end

%%
% On doit d�finir un seuil de d�tection :
seuil = 20000;
%% 
% On d�finit �galement un �cart entre 2 amplitudes �lev�e en dessous de 
% cet �cart, on consid�re qu'il s'agit du m�me choc et au dessus on consid�re 
% que ce sont 2 chocs s�par�s. 
% 
% C'est exprim� en ms pour plus de facilit� � convertir entre diff�rents 
% fichiers. Ainsi pour obtenir le nombre de bin d'�cart on fait : ecart * Fs / 
% 1000
% 
% En prenant 25 ms, les chocs r�p�titifs au dela de 40 Hz sont d�tect�s comme 
% un seul et m�me gros chocs de longue dur�e. 

ecartMS = 25;

ecartEnBins = ecartMS * FsSensors /1000;
%% 
% On doit donc prendre aussi comme param�tre la dur�e maximale d'un choc 
% si l'�cart ne permet pas de les s�parer.

dureeMax = 500;

dureeMaxEnBins = dureeMax * FsSensors / 1000;
%% 
% On doit stocker dans un tableau la position de tous les d�but D_L, ainsi 
% que le nombre pour commencer on en stocke 100 au max.

nbMax = 100;

nbChocs = zeros(2,1);
posLastChoc= zeros(2,1);
%inChoc= zeros(2,1); % permet de stocker si on est dans un choc ou entre 2 chocs

debuts = zeros(nbMax,2); % par convention on va de Gauche � Droite donc L en indice 1 et R en 2
fins = zeros(nbMax,2);

moyennes = zeros(nbMax,2);
ampAbs = zeros(nbMax,2);
amps = zeros(nbMax,2);
puissances = zeros(nbMax,2);

durees = zeros(nbMax,2);

timeShiftOnsets = zeros(nbMax,1);
timeShiftMax = zeros(nbMax,1);
ratioDuree = zeros(nbMax,1);
ratioAmps = zeros(nbMax,1);
ratioPuissances = zeros(nbMax,1);

%% 
% La boucle temporelle par sensor
for j=1:2 % 1 par sensor
    for i=1:tpsTotal
        if (abs(sensors(i,j))>seuil)
            if nbChocs(j)<1 % dans ce cas, il cr�e le premier sans se soucier du pr�c�dent
                nbChocs(j) = 1; % a voir si je ne rajoute pas un crit�re de dur�e minimale ...
                debuts(nbChocs(j),j) = i;
                posLastChoc(j) = i;
            %% dans ce cas soit le pr�c�dent est proche et il s'agit toujours du m�me soit loin
            elseif (posLastChoc(j) + ecartEnBins)>i % l� il est proche
                % on met � jour la position du lastChoc au cas o� �a soit le dernier
                posLastChoc(j) = i;
            elseif (posLastChoc(j) + dureeMaxEnBins)<i
                % On doit arr�ter le choc est trop long
                % ARRETER FONCTION !!!
                fins(nbChocs(j),j) = posLastChoc(j);
                durees(nbChocs(j),j) = fins(nbChocs(j),j) - debuts(nbChocs(j),j);
                moyennes(nbChocs(j),j) = mean(sensors(debuts(nbChocs(j),j):fins(nbChocs(j),j),j));
                ampAbs(nbChocs(j),j) = max(abs(sensors(debuts(nbChocs(j),j):fins(nbChocs(j),j),j))); 
                amps(nbChocs(j),j) = max(sensors(debuts(nbChocs(j),j):fins(nbChocs(j),j),j)) - ...
                    min(sensors(debuts(nbChocs(j),j):fins(nbChocs(j),j),j));
                puissances(nbChocs(j),j) = sum(abs(sensors(debuts(nbChocs(j),j):fins(nbChocs(j),j),j))); 
                
                % On cr�e le suivant
                nbChocs(j) =  nbChocs(j) + 1;
                debuts(nbChocs(j),j) = i;
                posLastChoc(j) = i;
            else % a priori l� c'est le premier du suivant
                fins(nbChocs(j),j) = posLastChoc(j);
                durees(nbChocs(j),j) = fins(nbChocs(j),j) - debuts(nbChocs(j),j);
                moyennes(nbChocs(j),j) = mean(sensors(debuts(nbChocs(j),j):fins(nbChocs(j),j),j));
                ampAbs(nbChocs(j),j) = max(abs(sensors(debuts(nbChocs(j),j):fins(nbChocs(j),j),j))); 
                amps(nbChocs(j),j) = max(sensors(debuts(nbChocs(j),j):fins(nbChocs(j),j),j)) - ...
                    min(sensors(debuts(nbChocs(j),j):fins(nbChocs(j),j),j));
                puissances(nbChocs(j),j) = sum(abs(sensors(debuts(nbChocs(j),j):fins(nbChocs(j),j),j))); 
                
                % On cr�e le suivant
                nbChocs(j) =  nbChocs(j) + 1;
                debuts(nbChocs(j),j) = i;
                posLastChoc(j) = i;
            end
        end
    end
end

%% C'est le moment d'afficher les chocs d�tect�s :
% un graphique resserr� sur les chocs pour montrer qu'on les a bien
% d�tect�s
debut = min(debuts(1,:));
chocsMax = max(nbChocs(:));
fin = max(max (fins(2:chocsMax,:)));
figure (1)
clf
for side=1:2
    subplot(2,1,side)
    plot(sensors(debut:fin,side))
    hold on
    plot(debuts(1:nbChocs(side)-1,side)-debut,seuil,'or')
    plot(fins(1:nbChocs(side)-1,side)-debut,seuil,'og')
end


%% Calcul des r�sultats
% L� une attention particuli�re car les pics ne sont pas tous d�tect�s par
% les 2 capteurs. Je dois donc cr�er une nouvelle matrice qui ne stocke que
% les pics transmis
transPeaks =  zeros(11,nbMax);
nbTransPeaks = 0;

for choc1=1:nbChocs(1)
    for choc2=1:nbChocs(2)
        if abs(debuts(choc1,1)-debuts(choc2,2))<900
            nbTransPeaks = nbTransPeaks+1;
            %% d�but
            transPeaks(1,nbTransPeaks) = debuts(choc1,1);
            transPeaks(2,nbTransPeaks) = debuts(choc2,2);
            %% fins
            transPeaks(3,nbTransPeaks) = fins(choc1,1);
            transPeaks(4,nbTransPeaks) = fins(choc2,2);
            %% dur�es
            transPeaks(5,nbTransPeaks) = durees(choc1,1);
            transPeaks(6,nbTransPeaks) = durees(choc2,2);
            %% timeshift
            transPeaks(7,nbTransPeaks) = debuts(choc1,1) - debuts(choc2,2);
            % 8 timeShiftMax = zeros(nbMax,1);
            %% ratio dur�e
            transPeaks(9,nbTransPeaks) = durees(choc1,1) / durees(choc2,2);
            %% ratio amplitudes
            transPeaks(10,nbTransPeaks) = amps(choc1,1) / amps(choc2,2);
            %% ratio puissances
            transPeaks(11,nbTransPeaks) = puissances(choc1,1) / puissances(choc2,2);
        end
    end
end

figure(2)
clf
plot(durees(:,1),puissances(:,1),'xr');
hold on 
plot(durees(:,2),puissances(:,2),'ob');
% timeShiftOnsets = zeros(nbMax,1);
% timeShiftMax = zeros(nbMax,1);
nbTransPeaks
transPeaks(7,1:nbTransPeaks)
mean(transPeaks(7,1:nbTransPeaks))
std(transPeaks(7,1:nbTransPeaks))
figure(3)
clf
plot(transPeaks(7,1:nbTransPeaks),transPeaks(11,1:nbTransPeaks),'xr');
hold on
plot(transPeaks(7,1:nbTransPeaks),transPeaks(9,1:nbTransPeaks),'ob');

figure(4) % zoom sur les donn�es
for side=1:2
    subplot(2,1,side)
    plot(sensors(min(min(transPeaks(1:2,1:nbTransPeaks))):max(max(transPeaks(3:4,1:nbTransPeaks))),side))
end

FsSensors
%% R�union des donn�es.
% Ce tableau est bien mais ce qui serait encore mieux serait de conserver 
% l'ensemble des �l�ments. Comment faire ? Le tableau transPeaks propose pour chaque ligne une valeur
% avant de faire �a, je dois faire tourner le programme et afficher les
% timeshifts en fonction de la position du choc. Une fois que j'ai bien vu
% qu'il y avait effectivement un timeshift diff�rent et stable en fonction
% de la position du coup de marteau. Alors dans ce cas, je pourrais faire �
% l'envert : Que le programme essaye de d�terminer la position.