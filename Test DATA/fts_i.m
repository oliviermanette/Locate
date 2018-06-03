%Affiche les valeurs brutes sur un seul graphique
debut = 30000;
longueur = 20000;
fin = debut+longueur;
seuil = 12000;
ecart = 300; %en dessous de cet écart, on considère 
figure(1)
subplot(2,1,1)
plot(D_L(debut:fin)>seuil)
subplot(2,1,2)
plot(D_R(debut:fin)>seuil)