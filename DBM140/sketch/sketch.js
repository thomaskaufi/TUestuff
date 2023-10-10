/*
Teachable Machine ml5 image example - modified from The Coding Train https://thecodingtrain.com/TeachableMachine/1-teachable-machine.html
and Andreas Refsgaard example here: https://editor.p5js.org/AndreasRef/sketches/gqE6vCsYf
*/

//remember to change to your URL !!!
let modelURL = "https://teachablemachine.withgoogle.com/models/KLFDplbUg/";
let label0 = ""
let label1 = label0;
let label2 = label1;
let confidence0 = 0.00;
let confidence1 = confidence0;
let confidence2 = confidence1;

let line_colour_R = 0;
let line_colour_G = 0;
let line_colour_B = 100;
let line_thickness = 3;
let interval = 100; //AKA speed
let timer = 0;
let x1 = 0.0;
let y1 = 0.0;
let x2 = 0.0;
let y2 = 0.0;

// STEP 1: Load the model
function preload() {
  const options = {
    probabilityThreshold: 0.00
  };
  classifier = ml5.soundClassifier(modelURL + "model.json", options);
  
}
function setup() {
  createCanvas(800, 800);
  background(200);
  // STEP 2: Start continous classifying from the microphone
  classifier.classify(gotResult);
  x1 = width / 2;
  y1 = height / 2;
  x2 = x1;
  y2 = y1;
}

function draw() {
  if (millis() - timer > interval) {
    timer = millis(); // Zero the timer to "now"

    // Move the endpoint of the line randomly
    let stepX = random(-50.0, 50.0);
    let stepY = random(-50.0, 50.0);
    x2 += stepX;
    y2 += stepY;

    // Constrain the line within the canvas
    x2 = constrain(x2, 0, width);
    y2 = constrain(y2, 0, height);

    // Draw the line
    stroke(line_colour_R, line_colour_G, line_colour_B); // Line color
    strokeWeight(line_thickness);    
    ellipse(x2, y2, line_thickness, line_thickness);

    // Update the starting point for the next frame
    x1 = x2;
    y1 = y2;
  }
  strokeWeight(0);
  textSize(18);
  fill(150);
  rect(0, 730, 190, 70);
  fill(255);

  text(label0, 0, 750);
  text(confidence0, 150, 750);
  text(label1, 0, 770);
  text(confidence1, 150, 770);
  text(label2, 0, 790);
  text(confidence2, 150, 790);
}

// STEP 3: Get the classification
function gotResult(error, results) {
  if (error) {
    console.error(error);
    return;
  }
  label0 = results[0].label;
  label1 = results[1].label;
  label2 = results[2].label;
  confidence0 = nf(results[0].confidence, 0, 2);
  confidence1 = nf(results[1].confidence, 0, 2);
  confidence2 = nf(results[2].confidence, 0, 2);
  
  if (label0 == "Normal breath"){
    line_colour_R = 100;
    line_colour_G = 200;
    line_colour_B = 300;
    }
  else if (label0 == "Heavy breath"){
    line_colour_R = 0;
    line_colour_G = 0;
    line_colour_B = 140;
    }
}
