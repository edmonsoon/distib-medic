#include <Stepper.h>
#define STEPS 64
const int mxbaril=2;
const int mxespvide=20;
const int pos_capt=1561; //position juste avant que le piston arrive sur le capteur (sans le couper) a definir
const int dist_coupe=937; //cran piston entre pos_capt et la guillotine. a definie
const int maxmedic=20; //nombre maximale d'un type de medicament à la même personne a definir

int Mot_guill[mxbaril]; //broche arduino de la guillotine de chaque baril
int Capt[mxbaril]; //broche arduino du capteur de chaque baril

int nbmedic[mxbaril]; //nombre de medicament à délivrer sur chaque baril (pour un patient)
int pospist[mxbaril]; //position du piston
int frontbas[mxbaril]; //position du dernier front bas
int num_esp_vide[mxbaril]; //numero espace vide sur une plaquette
int pos_esp_vide[mxbaril][mxespvide]; //position (piston) de chaque espace vide sur une plaquette
int etat[mxbaril]; //variable d'etat pour le suivit du programme.
boolean debut_plaquette[mxbaril]; //on est on debut de la plaquette
boolean obstacle[mxbaril]; //présence actuel d'un medic face au rayon lumineux

int numedic; //distribution des medicaments par multiplicité, numedic est le suivi de la multiplicité

Stepper Moteur(STEPS,6,7,8,9);
Stepper Moteur2(STEPS,2,3,4,5);

void setup(){
  Mot_guill[0]=9;  Capt[0]=10;
  Mot_guill[1]=12; Capt[1]=11;  
  Moteur.setSpeed(5);
  Moteur2.setSpeed(5);
  for (int i=0;i<mxbaril;i++) {    
    pinMode(Mot_guill[i],OUTPUT);
    digitalWrite(Mot_guill[i],LOW);
  }
  for (int i=0;i<mxbaril;i++) { 
    pinMode(Capt[i],INPUT); 
    etat[i]=0;
    initbaril(i); 
  }    
  Serial.begin(9600);  
  
}


void loop(){
 
  boolean pret_nv_distrib=true;
  for (int i=0;i<mxbaril;i++) { if (etat[i]!=0) {pret_nv_distrib=false;} }
  if (pret_nv_distrib){
    if (Serial.available()==0) {    
      for (int i=0;i<mxbaril;i++) { 
          nbmedic[i]=Serial.read();
          if (nbmedic[i]>0) {etat[i]=1;}
      }
      numedic=1;
    }else {numedic=1000;}
  } 
  for (int i=0;i<mxbaril;i++) { 
    if (nbmedic[i]>=numedic) {Medicsuiv(i);} //si le baril i doit distribuer un numedic_ieme medicament alors il execute
  }  
  boolean pret_prochain_numedic=true;
  for (int i=0;i<mxbaril;i++) { if (!(etat[i]==-1||etat[i]==0)) {pret_prochain_numedic=false;} }
  if (pret_prochain_numedic) {
    if (numedic<1000) {numedic++;}
    for (int i=0;i<mxbaril;i++) {
       if (nbmedic[i]>=numedic) {etat[i]=1;}
       else {etat[i]=0;}
    }   
  }  
}


void Medicsuiv(int b){
// distribue un medicament sur le baril  
// on avance d'un pas jusque ce qu'on  trouve une position de coupe sur la plaquette
//pendant ce temps on memorise les positions de coupe ulterieures
  if (etat[b]==1){
    if (!(
          (debut_plaquette[b]|| (pospist[b]<pos_esp_vide[b][num_esp_vide[b]+1]+dist_coupe))
            &&(pospist[b]<pos_capt+dist_coupe)
       )){etat[b]=2;} 
    else{
      avance_un_cran([b]);
      pospist[b]=pospist[b];
      if (pospist[b]<pos_capt){ 
        //test les fronts haut et bas jusqu'à arrivée du piston au capteur
        //après initule de tester (plaquette finie)
        if ((debut_plaquette[b])&&testobst(b)) {
          obstacle[b]=true;
          debut_plaquette[b]=false;
        }  
        if ((obstacle[b])&&!testobst(b)) {
          frontbas[b]=pospist[b];
          obstacle[b]=false;
        }  
        if(!(debut_plaquette[b])&&!(obstacle[b])&&(testobst(b))) {
          num_esp_vide[b]=num_esp_vide[b]+1;
          pos_esp_vide[b][num_esp_vide[b]]=(pospist[b]+frontbas[b])/2;
        }
      }
    }
  } 
  if (etat[b]==2) {
      if (pospist[b]>=pos_capt+dist_coupe) {
          retour_piston(b);
          initbaril(b);   
      }else{
        guillottine(b);    
      } 
      etat[b]=-1; 
  }  
}  

void initbaril(int b){
  for (int n=0;n<mxespvide;n++){
    for (int i=0;i<mxbaril;i++) { pos_esp_vide[b][i]=100000;}
  }  
  debut_plaquette[b]=true;
  pospist[b]=0;
  frontbas[b]=0;
  obstacle[b]=false;
}

void avance_un_cran(int b){
 int rohs=0;
   Moteur.step(1);
   rohs=rohs+1; 
}  

boolean testobst(int b){
  digitalRead( Capt[mxbaril]);

// renvoi si le pasage lumineux est bloqué ou non par un obstacle
// pour le baril b.
}  

void retour_piston(int b){
     Moteur.step(-64);
  
} 

void guillottine(int b){//verin un montée et une decente
   digitalWrite(Mot_guill[mxbaril], HIGH);
   delay(20);
   digitalWrite(Mot_guill[mxbaril], LOW);
}  




