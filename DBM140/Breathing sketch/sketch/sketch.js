/*

 BREATHING AS A MATTER OF SELF REFLECTION
 Based on code from Andreas Refsgaard & The Coding Train
 By Yaya Huang, Thomas Kaufmanas, Ben van Adrichem & Ilka van Zeijl
 
 */

//Variables for model input
let urlBreath = "https://teachablemachine.withgoogle.com/models/pk1iUsuc9/";
let urlTalking = "https://teachablemachine.withgoogle.com/models/9RU1T3Jpw/";
let urlPose = 'https://teachablemachine.withgoogle.com/models/Jbwo2dE1D/';
let recognizerPose;
let primaryBreath;
let primaryPose;
let labelsBreath;
let backconf = 0.00; //Confidence of background noise
let video;
let dev_toggle = false; //Toggle to show labels in visualization

//Visuals variables:
let red = 0;
let green = 0;
let blue = 70;
let strokeweight = 0;
let size = 10;
let cycle = 0;           //Store the current step of gradient cycle
let interval = 12000;    //Speed of gradient (interval of timer2). Blue to purple over 70 steps, once every 12 seconds = 15 minutes transition
let timer2 = 0;          //Timer for gradient cycle
let drawingSpeed = 100;  //Default drawing speed (Interval of timer)
let timer = 0;           //Timer for drawing
let x = 0;               //Dot positions
let y = 0;

async function setup() {
  createCanvas(800, 800);
  colorMode(RGB, 100);
  background(100);
  const recognizerBreath = await createBreath();
  const recognizerTalking = await createTalking();
  labelsBreath = recognizerBreath.wordLabels();

  recognizerPose = await tmPose.load(urlPose + "model.json", urlPose + "metadata.json");
  video = createCapture(VIDEO, getPose);

  recognizerBreath.listen(result => {
    confidencesBreath = [result.scores[0], result.scores[1], result.scores[2], result.scores[3]];
    primaryBreath = labelsBreath[confidencesBreath.indexOf(max(confidencesBreath))];
    if (primaryBreath == "Background Noise") {
      primaryBreath = "No breathing";
    }
  }
  );

  recognizerTalking.listen(result => {
    backconf = result.scores[0]
  }
  );
}

//Gather latest data from breathing pattern model
async function createBreath() {
  const recognizerBreath = speechCommands.create("BROWSER_FFT", undefined, urlBreath + "model.json", urlBreath + "metadata.json");
  await recognizerBreath.ensureModelLoaded();
  return recognizerBreath;
}

//Gather latest data from background noise model
async function createTalking() {
  const recognizerTalking = speechCommands.create("BROWSER_FFT", undefined, urlTalking + "model.json", urlTalking + "metadata.json");
  await recognizerTalking.ensureModelLoaded();
  return recognizerTalking;
}

//Gather latest data from posture model
async function getPose() {
  // Prediction #1: run input through posenet
  const {
    pose, posenetOutput
  }
  = await recognizerPose.estimatePose(video.elt, false);
  // Prediction 2: run input through teachable machine assification model
  const prediction = await recognizerPose.predict(posenetOutput, false, 4);
  const sortedPrediction = prediction.sort((a, b) => -a.probability + b.probability);
  primaryPose = sortedPrediction[0].className;
  getPose();
}

function draw() {

  //Labels for datainput in visualization. Can be toggled in line 19
  if (dev_toggle) {
    strokeWeight(0);
    fill(70);
    rect(0, 0, 230, 90);
    fill(100);
    textSize(18);
    text("Pose: " + primaryPose, 10, 20);
    text("Breath type: " + primaryBreath, 10, 40);
    text("backconf: " + nf(backconf, 1, 2), 10, 60);
    //print("We're in the drawing loop");
  }

  //Add data to browser console
  print("Pose: " + primaryPose + "     Breathing: " + primaryBreath + "     backconf: " + nf(backconf, 1, 2) + "     colour: " + red, green, blue)

    //Start of drawing loop. Enables to change speed of drawing without delaying the entire programme.
    if (millis() - timer > drawingSpeed) {
    timer = millis();  //Restart timer

    //Change quadrant of canvas depending on pose
    if (primaryPose === 'Slouch') {
      x = random(width / 2);
      y = random(0, height / 2);
    } else if (primaryPose === 'Straight back') {
      x = random(width / 2, width);
      y = random(0, height / 2);
    } else if (primaryPose === 'Lotus') {
      x = random(0, width / 2);
      y = random(height / 2, height);
    } else if (primaryPose === 'Lay down') {
      x = random(width / 2, width);
      y = random(height / 2, height);
    }

    //Change size strokeweight and speed depending on breathing pattern
    if (primaryBreath == 'Calm breathing') {
      size = 10; // Small dot size
      drawingSpeed = 1000;
      strokeweight = 5;
    } else if (primaryBreath == 'Normal') {
      size = 20; // Medium dot size
      drawingSpeed = 2000;
      strokeweight = 10;
    } else if (primaryBreath == 'Panting') {
      size = 30; // Big dot size
      drawingSpeed = 4000;
      strokeweight = 15;
    }

    //Add the transparent stroke, depending on if there is background noise
    if (backconf < 0.50) {
      strokeWeight(strokeweight);
    } else {
      noStroke();
    }
    
    //Color the dot, depending on the gradient
    fill(red, green, blue);
    stroke(red, green, blue, 50);
    
    //Draw the dot if there is a breathing classifyed
    if (primaryBreath != 'No breathing') {
      ellipse(x, y, size, size);
      print("drawing!")
    }
  } //end of draw timer

//Timer to change the gradient
  if (millis() - timer2 > interval) {
    timer2  = millis();
    if (cycle == 0) {    //red 0 -> 70
      if (red < 70) {
        red += 1;
      } else {
        cycle =+ 1
      }
    }
    if (cycle == 1) {    //red 70 -> 0
      if (red > 0) {
        red -= 1;
      } else {
        cycle = 0
      }
    }
  } //end of gradient-timer
  
} //End of Draw
