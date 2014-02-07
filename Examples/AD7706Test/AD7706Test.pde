/*
 * AD770X Library
 * sample code (AD7706)
 * Kerry D. Wong
 * http://www.kerrywong.com
 * 3/2011
 */

#include <AD770X.h>

AD770X ad7706(2.5);
double v;

void setup()
{
  Serial.begin(9600);

  ad7706.reset();
  ad7706.init(AD770X::CHN_AIN1);  
  ad7706.init(AD770X::CHN_AIN2);
}

void loop()
{
  v = ad7706.readADResult(AD770X::CHN_AIN1);
  Serial.print(v);

  v = ad7706.readADResult(AD770X::CHN_AIN2);
  Serial.print(" : ");
  Serial.println(v);
}
