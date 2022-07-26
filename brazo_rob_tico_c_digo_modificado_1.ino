// El objetivo del programa es que el usuario haga movimientos con el potenciómetro y se guarden para poder
// usarlos posteriormente. Para esto se va a programar el arduino en un modo para "Grabar" y otro modo para
// "Reproducir".

#include <Servo.h> // Incluimos la biblioteca de Servo.



// Declaramos 5 objetos Servo.
Servo Servo_0; // Controla el eje de la base
Servo Servo_1; // Controla el eslabón 1
Servo Servo_2; // Controla el eslabón 2
Servo Servo_3; // Controla el eslabón 3
Servo Gripper; // Controla la pinza

int cantMovimientos = 0; //movimientos que va haciendo.

// Declaración de variables globales.
int S0_pos, S1_pos, S2_pos, S3_pos, G_pos;
int P_S0_pos, P_S1_pos, P_S2_pos, P_S3_pos, P_G_pos;
int C_S0_pos, C_S1_pos, C_S2_pos, C_S3_pos, C_G_pos;
int POT_0,POT_1,POT_2,POT_3,POT_4;

int saved_data[700]; // Matriz para guardar los datos del área donde registramos la posición.

int array_index=0;
char incoming = 0;

int action_pos;
int action_servo;


void setup() {
	Serial.begin(9600);  // Iniciamos la comunicación serial.
	
	// Declaramos los pines de conexión de los servos.
	Servo_0.attach(3);
	Servo_1.attach(5);
	Servo_2.attach(6);
	Servo_3.attach(9);
	Gripper.attach(10);
	
	// Escribimos la posición inicial de cada Servo.
	Servo_0.write(70);
	Servo_1.write(100);
	Servo_2.write(110);
	Servo_3.write(10);
	Gripper.write(10);
	
	// Que el usuario presione R o P dependiendo del modo que elija.
	Serial.println("Ingrese:\nR guarda movimiento\nP ejecuta lo guardado.\nF pausa y muestra cantidad de piesas movidas."); 
}

// Definimos una función llamada Read_POT que lee los valores analógicos de los 5 potenciómetros
// y les asigna los valores de posición del Servo. El Arduino tiene un ADC de 8 bits
// que nos da una salida de 0 a 1023, pero los valores de posición de los servomotores varían de 
// solo 0 a 180. Además, dado que estos servomotores no son muy precisos, no es seguro llevarlos al
// extremo 0 o al extremo 180, por lo que establecemos 10-170 como nuestros límites. Usamos la función 
// de map() para convertir 0-1023 a 10-170 para los cinco motores como se muestra a continuación.

void Read_POT() {
	
   // Leemos el valor de las 5 entradas analógicas.
   POT_0 = analogRead(A0); 
   POT_1 = analogRead(A1); 
   POT_2 = analogRead(A2); 
   POT_3 = analogRead(A3); 
   POT_4 = analogRead(A4);
   S0_pos = map(POT_0,0,1024,10,170); // Mapea para el primer Servo (Base).
   S1_pos = map(POT_1,0,1024,10,170); // Mapea para el segundo Servo (Eslabón 1).
   S2_pos = map(POT_2,0,1024,10,170); // Mapea para el tercer Servo (Eslabón 2).
   S3_pos = map(POT_3,0,1024,10,170); // Mapea para el cuarto Servo (Eslabón 3).
   G_pos  = map(POT_4,0,1024,10,170); // Mapea para el quinto Servo (Pinza o Gripper).
}


// Cada POT corresponde a un motor individual, ya que el potenciómetro varía, debemos guardar
// la posición del motor y el número de motor dentro de la matriz saved_data.
// Empezamos definiendo el código para guardar la posición.

void Record() {
	// Puede pasar que el valor de servo_pos no se mantiene constante y, en ocasiones, aumenta 
	// o disminuye aleatoriamente. Entonces debemos programar el arduino para leer los valores 
	// POT dos veces y comparar ambos valores.
	// El valor se tomará como válido solo si ambos valores son iguales, de lo contrario, el valor 
	// se descartará. También hay que asegurarse de que el POT esté montado firmemente (soldarlo) 
	// al pin analógico del Arduino. Cualquier pérdida de conexión también causará vibraciones. 
	// Las variables P_x_pos se usan para guardar los valores antiguos y luego nuevamente los 
	// valores x_pos se leen y mapean usando la función Read_POT discutida anteriormente.
	
	Read_POT(); // Primero leemos los valores de los potenciómetros.
	
	// Guardamos los valores en las variables para compararlas después.
	P_S0_pos = S0_pos;
	P_S1_pos = S1_pos;
	P_S2_pos = S2_pos;
	P_S3_pos = S3_pos;
	P_G_pos  = G_pos;
	
	Read_POT(); // Leemos los valores de los potenciómetros por segunda vez.
	
	// Ahora hay que controlar si el valor de la posición del servomotor es válido. Además, después 
	// de controlar, tenemos que guardar el número de motor y la posición del motor en la matriz.
	// Se van a combinar dos matrices agregando un valor diferenciador al valor "pos" antes de 
	// guardarlo en la matriz.
	
	
	if (P_S0_pos == S0_pos) {  // Si el valor anterior y el valor actual son iguales.

    Servo_0.write(S0_pos);  // Pone el servo en esa posición.

    

    if (C_S0_pos != S0_pos) {  // Si el potenciómetro se movió.

      // Guarda la nueva posición en la matriz. Se agrega también un 0 para indicar que se trata del motor 0.
	  saved_data[array_index] = S0_pos + 0; 
	  
      array_index++; // Incrementa el índice de la matriz.

    }

    C_S0_pos = S0_pos; // Guarda el valor previo para chequear si el potenciómetro se movió. 

   }
   
   // Se repite lo mismo para los otros servomotores.

   if (P_S1_pos == S1_pos)

   {

    Servo_1.write(S1_pos);

    

    if (C_S1_pos != S1_pos)

    {

      saved_data[array_index] = S1_pos + 1000; // Se añade 1000 para el segundo servo, como un diferenciador.

      array_index++;

    }

    

    C_S1_pos = S1_pos;

   }


   if (P_S2_pos == S2_pos)

   {

    Servo_2.write(S2_pos);

    

    if (C_S2_pos != S2_pos)

    {

      saved_data[array_index] = S2_pos + 2000; // Se añade 2000 para el tercer servo.

      array_index++;

    }

    

    C_S2_pos = S2_pos;

   }


   if (P_S3_pos == S3_pos)

   {

    Servo_3.write(S3_pos); 

    

    if (C_S3_pos != S3_pos)

    {

      saved_data[array_index] = S3_pos + 3000;

      array_index++;

    }

    

    C_S3_pos = S3_pos;   

   }


   if (P_G_pos == G_pos)

   {

    Gripper.write(G_pos);

    

    if (C_G_pos != G_pos)

    {

      saved_data[array_index] = G_pos + 4000;

      array_index++;

    }

    

    C_G_pos = G_pos;

   }
   
   // Imprime los valores.
   
   Serial.print(S0_pos);  Serial.print("  "); 
   Serial.print(S1_pos); Serial.print("  "); 
   Serial.print(S2_pos); Serial.print("  "); 
   Serial.print(S3_pos); Serial.print("  "); 
   Serial.println(G_pos);
   
   Serial.print ("Index = "); Serial.println (array_index); 
   
   delay(100); 

}


// Una vez que se hayan guardado los datos, se puede cambiar al modo Play (Reproducir) introduciendo
// en la consola el caracter "P".
// Ahora se va a acceder a cada elemento guardado en la matriz y dividirlo para poder
// obtener el número y posición del motor para su posterior control.

void Play() {
	
	// Con la función for vamos a recorrer todos los elementos de la matriz.
	// Después usamos dos variables: action_servo y action_pos para obtener
    // el número de servomotores a controlar y su posición respectivamente.
	// Para obtener el número de servomotor tenemos que dividirlo por 1000 (porque en
	// la matriz guardamos los valor con +1000 para identificar cada motor), y para 
	// obtener la posición necesitamos los últimos tres dígitos, que se pueden obtener
	// tomando un módulo.
	
	for (int Play_action=0; Play_action<array_index; Play_action++) {

    action_servo = saved_data[Play_action] / 1000;

    action_pos = saved_data[Play_action] % 1000;
	
	
	// Ahora con la función switch vamos a usar el número de cada servo y moverlo a la posición
	// que obtuvimos.
	
    switch(action_servo){

      case 0: // Motor 0 (Base)

        Servo_0.write(action_pos);

      break;


      case 1: // Motor 1 (Eslabón 1)

        Servo_1.write(action_pos);

      break;


      case 2:  // Motor 2 (Eslabón 2)

        Servo_2.write(action_pos);

      break;


      case 3:  // Motor 3 (Eslabón 3)

        Servo_3.write(action_pos);

      break;


      case 4:  // Motor 4 (Eslabón 4)

        Gripper.write(action_pos);

      break;

    }


    delay(50);

    

  }

}


// Por último tenemos que verificar lo que el usuario ha ingresado a través del monitor serie.
// La variable entrante se utiliza para contener el valor del usuario. Si se ingresa 'R', se 
// activará el modo de grabación y si se presiona 'P', se ejecutará el modo de reproducción 
// mediante declaraciones condicionales como se muestra a continuación. 

void loop() {


if (Serial.available() > 1) // Si se recibe algo de la consola.

{

incoming = Serial.read();

if (incoming == 'R' || incoming == 'r'){
cantMovimientos++;
Serial.println("Grabando secuencia.");
}
if (incoming == 'P' || incoming == 'p')
{
cantMovimientos++;
Serial.println("Repeticion de secuencia.");
}
}


if (incoming == 'R' || incoming == 'r') // Si se seleccionó "R".
{
  
Record();
}

if (incoming == 'P' || incoming == 'p') // Si se seleccionó "P".
{
  
    Play();
  
}
  if (incoming == 'F' || incoming == 'f'){
    Serial.println("Se movieron: "); Serial.println(cantMovimientos);
    incoming = 'a';
  }
}
