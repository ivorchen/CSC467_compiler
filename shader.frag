
{ 
   vec4 temp;
   int a=0;
   int b=-a;
   int c=10;
   int d=12;
   int g=d-c;
   bool h= false;
if (!true && h){
   temp[0] = gl_Color[0] * gl_FragCoord[0];
   temp[1] = gl_Color[1] * gl_FragCoord[1];
   temp[2] = gl_Color[2];
   temp[3] = gl_Color[3] * gl_FragCoord[0] * gl_FragCoord[1];
}
else{
   temp = gl_Color;
}

  g=c+d;
  gl_FragColor = temp;

}

