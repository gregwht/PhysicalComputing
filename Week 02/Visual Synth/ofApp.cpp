#include "ofApp.h"


//--------------------------------------------------------------
//----------------------  Params -------------------------------
//--------------------------------------------------------------
Params param;        //Definition of global variable

void Params::setup() {
	eCenter = ofPoint( ofGetWidth() / 2, ofGetHeight() / 2 );
	eRad = 100;
	velRad = 0;
	lifeTime = 0.85;

	rotate = 0;
	force = 1000;
	spinning = 1000;
	friction = 0.05;
    invert = 0;
}

//--------------------------------------------------------------
//----------------------  Particle  ----------------------------
//--------------------------------------------------------------
Particle::Particle() {
	live = false;
}

//--------------------------------------------------------------
ofPoint randomPointInCircle( float maxRad ){
	ofPoint pnt;
	float rad = ofRandom( 0, maxRad );
	float angle = ofRandom( 0, M_TWO_PI );
	pnt.x = cos( angle ) * rad;
	pnt.y = sin( angle ) * rad;
	return pnt;
}

//--------------------------------------------------------------
void Particle::setup() {
	pos = param.eCenter + randomPointInCircle( param.eRad );
	vel = randomPointInCircle( param.velRad );
	time = 0;
	lifeTime = param.lifeTime;
	live = true;
    
}

//--------------------------------------------------------------
void Particle::update( float dt ){
	if ( live ) {
		//Rotate vel
		vel.rotate( 0, 0, param.rotate * dt );

		ofPoint acc;         //Acceleration
		ofPoint delta = pos - param.eCenter;
		float len = delta.length();
		if ( ofInRange( len, 0, param.eRad ) ) {
			delta.normalize();

			//Attraction/repulsion force 
			acc += delta * param.force;

			//Spinning force
			acc.x += -delta.y * param.spinning;
			acc.y += delta.x * param.spinning;
		}
		vel += acc * dt;            //Euler method
		vel *= (1-param.friction);  //Friction

		//Update pos
		pos += vel * dt;    //Euler method

		//Update time and check if particle should die
		time += dt;
		if ( time >= lifeTime ) {
			live = false;   //Particle is now considered as died
		}
	}
}

//--------------------------------------------------------------
void Particle::draw(){
	if ( live ) {
		//Compute size
		float size = ofMap( 
			fabs(time - lifeTime/2), 0, lifeTime/2, 3, 1 );

		//Compute color
		
        if (param.invert == 1){
            ofColor color = ofColor::black;
            float hue = ofMap( time, 0, lifeTime, 128, 255 );
            color.setHue( hue );
            ofSetColor( color );
        }
        
        else {
            ofColor color = ofColor::white;
            float hue = ofMap( time, 0, lifeTime, 128, 255 );
            color.setHue( hue );
            ofSetColor( color );
        }


		ofCircle( pos, size );  //Draw particle
	}
}

//--------------------------------------------------------------
//----------------------  ofApp  -----------------------------
//--------------------------------------------------------------
void ofApp::setup(){
	ofSetFrameRate( 60 );	//Set screen frame rate

	//Allocate drawing buffer
	int w = ofGetWidth();
	int h = ofGetHeight();
	fbo.allocate( w, h, GL_RGB32F_ARB );

	//Fill buffer with white color
	fbo.begin();
    if (param.invert == 1) {ofBackground(255, 255, 255);}
    else {ofBackground(0, 0, 0);}    //<------ BACKGROUND COLOUR 1/3
	fbo.end();

	//Set up parameters
	param.setup();		//Global parameters
	history = 0.9;
	bornRate = 1500;
	
	bornCount = 0;
	time0 = ofGetElapsedTimef();

	//GUI
	interf.setup();
    interf.addSlider( "rate", &bornRate, 0, 3000 );
    interf.addSlider( "lifeTime", &param.lifeTime, 0.1, 5 );
    interf.addSlider( "history", &history, 0, 1 );

	interf.addSlider( "eRad", &param.eRad, 0, 800 );
    interf.addSlider( "velRad", &param.velRad, 0, 400 );
    interf.addSlider( "rotate", &param.rotate, -500, 500 );
    interf.addSlider( "spinning", &param.spinning, -1000, 1000 );
    interf.addSlider( "force", &param.force, -1000, 1000 );
	interf.addSlider( "friction", &param.friction, 0, 0.1 );
	
	drawInterface = true;
    
    // Greg's additions
    strobe = false;
    receiver.setup(PORT);
    
    // -----Arduino
    potValue = 0;
    // replace the string below with the serial port for your Arduino board
    ard.connect("/dev/cu.usbmodemFA131", 57600);
    // listen for EInitialized notification. this indicates that
    // the arduino is ready to receive commands and it is safe to
    // call setupArduino()
    ofAddListener(ard.EInitialized, this, &ofApp::setupArduino);
    bSetupArduino	= false;	// flag so we setup arduino when its ready, you don't need to touch this :)

}

//--------------------------------------------------------------
void ofApp::update(){
	//Compute dt
	float time = ofGetElapsedTimef();
	float dt = ofClamp( time - time0, 0, 0.1 );
	time0 = time;

	//Delete inactive particles
	int i=0;
	while (i < p.size()) {
		if ( !p[i].live ) {
			p.erase( p.begin() + i );
		}
		else {
			i++;
		}
	}

	//Born new particles
	bornCount += dt * bornRate;      //Update bornCount value
	if ( bornCount >= 1 ) {          //It's time to born particle(s)
		int bornN = int( bornCount );//How many born
		bornCount -= bornN;          //Correct bornCount value
		for (int i=0; i<bornN; i++) {
			Particle newP;
			newP.setup();            //Start a new particle
			p.push_back( newP );     //Add this particle to array
		}
	}

	//Update the particles
	for (int i=0; i<p.size(); i++) {
		p[i].update( dt );
	}
    
    //---OSC----------------------------------------------
    while(receiver.hasWaitingMessages()){
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(&m);
        
        
        // check for 2D OSCemote message
        // Documentation from https://code.google.com/p/oscemote/wiki/ListOfOSCMessages
        if(m.getAddress() == "/1/rate"){
            float x = m.getArgAsFloat(0);
            bornRate = ofMap(x, 0., 1., 0, 3000);
            cout << bornRate << endl;
        }
        
        if(m.getAddress() == "/1/lifetime"){
            float x = m.getArgAsFloat(0);
            param.lifeTime = ofMap(x, 0., 1., 0.1, 5);
            cout << param.lifeTime << endl;
        }
        
        if(m.getAddress() == "/1/history"){
            float x = m.getArgAsFloat(0);
            history = ofMap(x, 0., 1., 0, 1);
            cout << history << endl;
        }
        
        if(m.getAddress() == "/1/eRad"){
            float x = m.getArgAsFloat(0);
            param.eRad = ofMap(x, 0., 1., 0, 800);
            cout << param.eRad << endl;
        }
        
        if(m.getAddress() == "/1/velRad"){
            float x = m.getArgAsFloat(0);
            param.velRad = ofMap(x, 0., 1., 0, 400);
            cout << param.velRad << endl;
        }
        
        if(m.getAddress() == "/1/rotate"){
            float x = m.getArgAsFloat(0);
            param.rotate = ofMap(x, 0., 1., 0, 500);
            cout << param.rotate << endl;
        }
        
        if(m.getAddress() == "/1/spinning"){
            float x = m.getArgAsFloat(0);
            param.spinning = ofMap(x, 0., 1., 0, 1000);
            cout << param.spinning << endl;
        }
        
        if(m.getAddress() == "/1/force"){
            float x = m.getArgAsFloat(0);
            param.force = ofMap(x, 0., 1., -1000, 1000);
            cout << param.force << endl;
        }
        
        if(m.getAddress() == "/1/friction"){
            float x = m.getArgAsFloat(0);
            param.friction = ofMap(x, 0., 1., 0.1, 0.);
            cout << param.friction << endl;
        }
        
        if(m.getAddress() == "/1/push4"){
            float x = m.getArgAsFloat(0);
            if (x == 1){strobe = true;}
            cout << strobe << endl;
        }
        
        if(m.getAddress() == "/1/push2"){
            float x = m.getArgAsFloat(0);
            if (x == 1){
                if (param.invert == true){
                    param.invert = false;
                }
                else {
                    param.invert = true;
                }
            }
            
            cout << param.invert << endl;
        }


//
//        if(m.getAddress() == "/2/fader2"){
//            
//            float y = m.getArgAsFloat(0);
//            vy = 360 * y + 180;
//        }
        
        else{
            // unrecognized message: display on the bottom of the screen
            string msg_string;
            msg_string = m.getAddress();
            msg_string += ": ";
            
            for(int i = 0; i < m.getNumArgs(); i++){
                
                // get the argument type
                msg_string += m.getArgTypeName(i);
                msg_string += ":";
                
                // display the argument - make sure we get the right type
                if(m.getArgType(i) == OFXOSC_TYPE_INT32){
                    
                    msg_string += "int: " + ofToString(m.getArgAsInt32(i));
                }
                
                
                else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
                    
                    msg_string += "Float: " + ofToString(m.getArgAsFloat(i));
                }
                
                else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
                    
                    msg_string += "String: " + m.getArgAsString(i);
                }
                
                else{
                    
                    msg_string += "unknown";
                }
            }
            cout << msg_string << endl;
        }
        
    }

    // ----- Arduino
    updateArduino();
}

//--------------------------------------------------------------
void ofApp::setupArduino(const int & version) {
    
    // remove listener because we don't need it anymore
    ofRemoveListener(ard.EInitialized, this, &ofApp::setupArduino);
    
    // it is now safe to send commands to the Arduino
    bSetupArduino = true;
    
    // print firmware name and version to the console
    ofLogNotice() << ard.getFirmwareName();
    ofLogNotice() << "firmata v" << ard.getMajorFirmwareVersion() << "." << ard.getMinorFirmwareVersion();
    
    // set pin A0 to analog input
    ard.sendAnalogPinReporting(0, ARD_ANALOG);
    
    // Listen for changes on the digital and analog pins
    ofAddListener(ard.EAnalogPinChanged, this, &ofApp::analogPinChanged);
}

//--------------------------------------------------------------
void ofApp::updateArduino(){
    
    // update the arduino, get any data or messages.
    // the call to ard.update() is required
    ard.update();
    
    // do not send anything until the arduino has been set up
    if (bSetupArduino) {
        // fade the led connected to pin D11
        ard.sendPwm(11, (int)(128 + 128 * sin(ofGetElapsedTimef())));   // pwm...
    }
    
}


//--------------------------------------------------------------
void ofApp::analogPinChanged(const int & pinNum) {

    // analog pin event handler, called whenever an analog pin value has changed
    
    // do something with the analog input. here we're simply going to print the pin number and
    // value to the screen each time it changes
    potValue = ard.getAnalog(pinNum);
    
    param.eRad = ofMap(potValue, 0, 1024, 0, 650);
    
    bornRate = ofMap(potValue, 0, 1024, 0, 3000);
    
    param.rotate = ofMap(potValue, 0., 1024, -300, 300);
    
    
    cout << "eRad: " << param.eRad << "   " << "bornRate: " << bornRate << "   " <<  "rotate: " << param.rotate << endl;
}



//--------------------------------------------------------------
void ofApp::draw(){
	
    if (param.invert == 1) { ofBackground( 255, 255, 255 );}
    else {ofBackground( 0, 0, 0 );}  // <----- BACKGROUND COLOUR 2/3

	//1. Drawing to buffer
	fbo.begin();

	//Draw semi-transparent white rectangle
	//to slightly clearing a buffer (depends on history value)

	ofEnableAlphaBlending();         //Enable transparency

	float alpha = (1-history) * 255;
    if (param.invert == 1){ ofSetColor( 255,255,255, alpha );}
    else{ofSetColor( 0,0,0, alpha );} // <------ BACKGROUND COLOUR 3/3
	ofFill();
	ofRect( 0, 0, ofGetWidth(), ofGetHeight() );

	ofDisableAlphaBlending();        //Disable transparency

	//Draw the particles
	ofFill();
	for (int i=0; i<p.size(); i++) {
		p[i].draw();
	}

	fbo.end();

	//2. Draw buffer on the screen
	ofSetColor( 255, 255, 255 );
	fbo.draw( 0, 0 );

	//GUI
	if ( drawInterface ) {	
	    //Draw text
		ofSetColor( 128, 128, 128 );
		ofDrawBitmapString( "Keys: Enter-hide/show GUI, Space-screenshot, 1,2,...,9-load preset, Shift+1,2,...9-save preset", 20, 20 );
		ofDrawBitmapString( "Particles: " + ofToString( p.size() ), 20, 40 );

		//Draw sliders
		interf.draw();

		//Draw emitter as a circle
		ofSetCircleResolution( 50 );
		ofNoFill();
		ofSetColor( 128, 128, 128 );
		ofCircle( param.eCenter, param.eRad );
		ofSetCircleResolution( 20 );
	}
    
    if (strobe){
        
        if (param.invert){
        
            ofBackground(0);
        }
        else {
            
            ofBackground(255);
        }
        
        strobe = false;
    }
}

//--------------------------------------------------------------
//----------------------  GUI ----------------------------------
//--------------------------------------------------------------
void Interface::setup(){
    selected = -1;
}

void Interface::addSlider( string title, float *value, float minV, float maxV ){
    Slider s;
    s.title = title;
    s.rect = ofRectangle( 20, 60 + slider.size() * 40, 150, 30 );
    s.value = value;
    s.minV = minV;
    s.maxV = maxV;
    slider.push_back( s );
}

void Interface::draw(){
    for (int i=0; i<slider.size(); i++) {
        Slider &s = slider[i];
        ofRectangle r = s.rect;
		ofFill();
		ofSetColor( 255, 255, 255 );
		ofRect( r );
	    ofSetColor(128);
        ofNoFill();
        ofRect( r );
        ofFill();
        float w = ofMap( *s.value, s.minV, s.maxV, 0, r.width );
        ofRect( r.x, r.y + 15, w, r.height - 15 );
        ofDrawBitmapString( s.title + " " + ofToString( *s.value, 2 ), r.x + 5, r.y + 12 );
    }
}

void Interface::mousePressed( int x, int y ){
    for (int i=0; i<slider.size(); i++) {
        Slider &s = slider[i];
        ofRectangle r = s.rect;
        if ( ofInRange( x, r.x, r.x + r.width ) && ofInRange( y, r.y, r.y + r.height ) ) {
            selected = i;
            *s.value = ofMap( x, r.x, r.x + r.width, s.minV, s.maxV, true );
        }
    }
}

void Interface::mouseDragged( int x, int y ){
    if ( selected >= 0 ) {
        Slider &s = slider[selected];
        ofRectangle r = s.rect;
        *s.value = ofMap( x, r.x, r.x + r.width, s.minV, s.maxV, true );
    }
}

void Interface::mouseReleased (int x, int y ){
    selected = -1;
}

//--------------------------------------------------------------
//For saving/loading presets we use very simple method:
//just pack all sliders values into list and save/load it from file.
//Its very simple, but is not practical, because when saved, 
//you can not change the number of sliders and these order.
//The best solution is using ofxXmlSettings - it lets write sliders values
//and specify these names

void Interface::save( int index )		
{
	vector<string> list;
	for (int i=0; i<slider.size(); i++) {
		list.push_back( ofToString( *slider[i].value ) );
	}
	string text = ofJoinString( list," " );
	string fileName = "preset" + ofToString( index ) + ".txt";
    ofBuffer buffer = ofBuffer( text );
	ofBufferToFile( fileName, buffer );
}

//--------------------------------------------------------------
void Interface::load( int index )
{
	string fileName = "preset" + ofToString( index ) + ".txt";
	string text = string( ofBufferFromFile( fileName ) );
	vector<string> list = ofSplitString( text, " " );

	if ( list.size() == slider.size() ) {
		for (int i=0; i<slider.size(); i++) {
			*slider[i].value = ofToFloat( list[i] );
		}
	}
}

//--------------------------------------------------------------
//----------------------  ofApp again  -----------------------
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if ( key == OF_KEY_RETURN ) {	//Hide/show GUI
		drawInterface = !drawInterface;
	}

	if ( key == ' ' ) {		//Grab the screen image to file
		ofImage image;	
		image.grabScreen( 0, 0, ofGetWidth(), ofGetHeight() );	

		//Select random file name
		int n = ofRandom( 0, 1000 );
		string fileName = "screen" + ofToString( n ) + ".png";

		image.saveImage( fileName );	
	}

	//Load presets
	if ( key == '1' ) { interf.load( 1 ); }
	if ( key == '2' ) { interf.load( 2 ); }
	if ( key == '3' ) { interf.load( 3 ); }
	if ( key == '4' ) { interf.load( 4 ); }
	if ( key == '5' ) { interf.load( 5 ); }
	if ( key == '6' ) { interf.load( 6 ); }
	if ( key == '7' ) { interf.load( 7 ); }
	if ( key == '8' ) { interf.load( 8 ); }
	if ( key == '9' ) { interf.load( 9 ); }

	//Save presets
	if ( key == '!' ) { interf.save( 1 ); }
	if ( key == '@' ) { interf.save( 2 ); }
	if ( key == '#' ) { interf.save( 3 ); }
	if ( key == '$' ) { interf.save( 4 ); }
	if ( key == '%' ) { interf.save( 5 ); }
	if ( key == '^' ) { interf.save( 6 ); }
	if ( key == '&' ) { interf.save( 7 ); }
	if ( key == '*' ) { interf.save( 8 ); }
	if ( key == '(' ) { interf.save( 9 ); }
    
    if (key == 'p'){
        
        strobe = true;
    }
    
    if (key == 'i'){
        
        if (param.invert == 0){
            param.invert = 1;
        }
        else {
            param.invert = 0;
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if ( drawInterface ) {
		interf.mouseDragged( x, y );     
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if ( drawInterface ) {
	    interf.mousePressed( x, y );     
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    interf.mouseReleased( x, y );    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}