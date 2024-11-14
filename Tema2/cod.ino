int zi;
int luna;
int an;
int ora;
int minut;
int secunda;

void modificare(String informatie)
{
  zi=informatie[0]-48;
  zi=zi*10+ (informatie[1]-48);

  luna=informatie[3]-48;
  luna=luna*10+(informatie[4]-48);

  an=informatie[6]-48;
  an = an*10+(informatie[7]-48);
  an = an*10+(informatie[8]-48);
  an = an*10+(informatie[9]-48);

  ora=informatie[11]-48;
  ora=ora*10+(informatie[12]-48);

  minut=informatie[14]-48;
  minut=minut*10+(informatie[15]-48);

  secunda= informatie[17]-48;
  secunda= secunda*10+(informatie[18]-48);
}

void update2()
{
  secunda++;
  if(secunda==60)
  {
    secunda=0;
    minut++;
  }
  if(minut==60)
  {
    minut=0;
    ora++;
  }
  if(ora==24)
  {
    ora=0;
    zi++;
  }
  if(zi==30)
  {
    zi=1;
    luna++;
  }
  if(luna==12)
  {
    luna=1;
    an++;
  }
}

void display2()
{
  if(zi<10)
  {
    Serial.print(0);
  }
  Serial.print(zi);
  Serial.print(".");
  if(luna<10)
  {
    Serial.print(0);
  }
  Serial.print(luna);
  Serial.print(".");
  Serial.print(an);
  Serial.print(" ");
  if(ora<10)
  {
    Serial.print(0);
  }
  Serial.print(ora);
  Serial.print(":");
  if(minut<10)
  {
    Serial.print(0);
  }
  Serial.print(minut);
  Serial.print(":");
  if(secunda<10)
  {
    Serial.print(0);
  }
  Serial.print(secunda);
  Serial.print("\n");
}
String Read()
{
  while(Serial.available()==0){}
  String informatie;
  informatie=Serial.readString();
  return informatie;
}
void setup()
{
  Serial.begin(115200);
  modificare(Read());
}
void loop()
{
  display2();
  update2();
  delay(1000);
}