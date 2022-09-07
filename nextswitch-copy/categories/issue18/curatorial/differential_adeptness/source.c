#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int disx=25,disy=25,dismax=5,comdismax=5;
int readlength=4, matrixsize=200, resolution=2;
int mem1count=0,mem2count=0,mem1countmax=20,mem2countmax=20,grmax=100;
int system1comcount=0,system2comcount=0,system1commax=20,system2commax=20;
int lightswitch=0,flatswitch=0,layerswitch=0;
float sizeretard=2.7,tailsize=0.7;
float spot0cone=70,spot1cone=70;
float ground[300][300][2],ground2[300][300][2],composite[300][300][2],composite2[300][300][2];
float material_specular[] = { 1.0, 1.0, 1.0, 0.0 }; // White highlights
float ambient_color[] = { 0.2, 0.2, 0.2, 0.0 };
float light0_position[] = { -50.0, -50.0, 20.0, 1.0 }; // local
float light0_color[] = { 1.0, 0.0, 0.0, 1.0 };
float light1_position[] = { -50.0, -50.0, 20.0, 1.0 };
float light1_color[] = { 0.0, 1.0, 0.0, 1.0 };
int system1[10],system2[10];
int system1mem[31][5],system2mem[31][5];
int environmatrix[300][300][1],system1matrix[300][300][1],system2matrix[300][300][1];
int system1grav[300][300][2];
char document1[40],document2[40],document3[40];


FILE *recordfp;
FILE *environfp;
FILE *system1fp;
FILE *system2fp;

float eyex=100,eyey=100,eyez=150,lookx=100,looky=100;

static GLuint cubeList;

GLint motionx = 100;
GLint motiony = 100;
GLint mouseViewpointControl = 1;

void displace(int,int);
void comdisplace(int,int);
void textreader(void);
void move(int);
void interpret(int,int,int,int,int);
void remember(int,int,int);
void rebound(void);
void compare(int,int,int);
void communication(int,int,int);
void printout(int);
void ritedata(void);

void init(void)
{
	printf("Enter environment file.\n");
	scanf("%s",document1);
	printf("Enter system file #1.\n");
	scanf("%s",document2);
	printf("Enter system file #2.\n");
	scanf("%s",document3);
	while ((environfp=fopen(document1,"r"))==NULL)
		{
			printf("Cant open environment file.\n");
			printf("Enter environment file.\n");
			scanf("%s",document1);
			//exit(0);
		}
	while ((system1fp=fopen(document2,"r"))==NULL)
		{
			printf("Cant open system file #1.\n");
			printf("Enter system file #1.\n");
			scanf("%s",document2);
			//exit(0);
		}
	while ((system2fp=fopen(document3,"r"))==NULL)
		{
			printf("Cant open system file #2.\n");
			printf("Enter system file #2.\n");
			scanf("%s",document3);
			//exit(0);
		}
	rewind(system1fp);
	rewind(system2fp);
	rewind(environfp);
	textreader();
	srand((unsigned int) time(0));
	system1[0]=(rand()%matrixsize-20)+10;
    system1[1]=(rand()%matrixsize-20)+10;
	system1[2]=0;
	system1[3]=1;
	system1[4]=1;
	system1[5]=10;
	system2[0]=(rand()%matrixsize-20)+10;
    system2[1]=(rand()%matrixsize-20)+10;
	system2[2]=0;
	system2[3]=-1;
	system2[4]=-1;
	system2[5]=10;
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Set up lighting
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glMaterialf(GL_FRONT, GL_SHININESS, 50.0f); // For all materials
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_color);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spot0cone);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, spot1cone);
	GLfloat spot_direction0[] = { 0.0, 0.0, -1.0 };
	GLfloat spot_direction1[] = { 0.0, 0.0, -1.0 };
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction0);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction1);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_color);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_color);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_color);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_color);
    //glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glClearColor(0.0,0.0,0.0,0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(80.0,1.0,1.0,500.0);
    gluLookAt(eyex, eyey, eyez,lookx,looky,0,0.0,1.0,0.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	cubeList = glGenLists(1);
	glNewList(cubeList, GL_COMPILE);
	glBegin(GL_QUAD_STRIP);
	glVertex3f(-10,-10,0);
	glVertex3f(-10,10,0);
	glVertex3f(10,-10,0);
	glVertex3f(10,10,0);
	glEnd();
	//glutSolidCube(20);
	glEndList();

}

void display(void)
{
	int a,b;
	float bb,cc;
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLfloat groundcolor[] = { 0.2, 0.2, 0.8, 1.0 };
	GLfloat emission1color[] = { 0.15, 0.0, 0.0, 1.0 };
	GLfloat emission2color[] = { 0.0, 0.15, 0.0, 1.0 };
	GLfloat emission3color[] = { 0.0, 0.0, 0.15, 1.0 };
	GLfloat emission4color[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat solidcolor[] = { 0.0, 0.0, 0.0, 0.0 };
	GLfloat objectcolor[] = { 0.2, 0.2, 0.2, 1.0 };
	if (lightswitch==1)
		{
			light0_position[0] = system1[0];
			light0_position[1] = system1[1];
			light1_position[0] = system2[0];
			light1_position[1] = system2[1];
			cc = ground[system1[0]][system1[1]][0];
			if (cc>=-30&&cc<0)
				light0_color[1]=sqrt((cc/30)*(cc/30));
			if (bb<-30)
				light0_color[2]=sqrt((cc/100)*(cc/100));
			if (cc<=30&&cc>0)
				{
					light0_color[0]=cc/30;
					light0_color[1]=cc/30;
				}
			if (cc>30)
				light0_color[0]=cc/100;
			cc = ground[system2[0]][system2[1]][0];
			if (cc>=-30&&cc<0)
				light1_color[1]=sqrt((cc/30)*(cc/30));
			if (bb<-30)
				light1_color[2]=sqrt((cc/100)*(cc/100));
			if (cc<=30&&cc>0)
				{
					light1_color[0]=cc/30;
					light1_color[1]=cc/30;
				}
			if (cc>30)
				light1_color[0]=cc/100;
		}
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spot0cone);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, spot1cone);
	GLfloat spot_direction0[] = { 0.0, 0.0, -1.0 };
	GLfloat spot_direction1[] = { 0.0, 0.0, -1.0 };
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction0);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction1);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_color);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_color);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_color);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_color);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(80.0,1.0,1.0,500.0);
    gluLookAt(eyex, eyey, eyez,lookx,looky,0,0.0,1.0,0.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	move(1);
	move(2);
	interpret(1,system1[0]-system1[5],system1[1]-system1[5],system1[0]+system1[5],system1[1]+system1[5]);
	interpret(2,system2[0]-system2[5],system2[1]-system2[5],system2[0]+system2[5],system2[1]+system2[5]);
	remember(1,system1[0],system1[1]);
	remember(2,system2[0],system2[1]);
	rebound();
	if (system1comcount>=system1commax)
		communication(1,system1[0],system1[1]);
	else
		system1comcount+=1;
	if (system2comcount>=system2commax)
			communication(2,system2[0],system2[1]);
	else
		system2comcount+=1;
	compare(1,system1[0],system1[1]);
	compare(2,system2[0],system2[1]);
	/*glPushMatrix();
	glTranslatef(system1[0],system1[1],0);
	glMaterialfv(GL_FRONT, GL_EMISSION, emission1color);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, objectcolor);
	glCallList (cubeList);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(system2[0],system2[1],0);
	glMaterialfv(GL_FRONT, GL_EMISSION, emission2color);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, objectcolor);
	glCallList (cubeList);
	glPopMatrix();*/
    glEnable (GL_BLEND);
    glDepthMask (GL_FALSE);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_EMISSION, emission4color);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, groundcolor);
	for(a=0;a<=matrixsize;a=a+resolution)
		{
		glBegin(GL_QUAD_STRIP);
			for(b=0;b<=matrixsize;b++)
				{
					if (layerswitch==1)
						bb=system1matrix[b][a][0];
					else if (layerswitch==2)
						bb=system2matrix[b][a][0];
					else if (layerswitch==3)
						bb=composite2[b][a][0];
					else if (layerswitch==4)
						bb=environmatrix[b][a][0];
					else if (layerswitch==5)
						bb=20*composite[b][a][1];
					else
						bb=ground[b][a][0];
					if (bb>=-30&&bb<0)
						emission4color[1]=sqrt((bb/30)*(bb/30));
					if (bb<-30)
						emission4color[2]=sqrt((bb/100)*(bb/100));
					if (bb<=30&&bb>0)
						{
							emission4color[0]=bb/30;
							emission4color[1]=bb/30;
						}
					if (bb>30)
						emission4color[0]=bb/100;
					glMaterialfv(GL_FRONT, GL_EMISSION, emission4color);
					if (flatswitch==1)
						{
							ground2[b][a][0]=0;
							ground2[b][a+1][0]=0;
						}
					else
						{
							ground2[b][a][0]=ground[b][a][0];
							ground2[b][a+1][0]=ground[b][a+1][0];
						}
					glVertex3f(b,a,ground2[b][a][0]);
					glVertex3f(b,a+1,ground2[b][a+1][0]);
					emission4color[0]=0;
					emission4color[1]=0;
					emission4color[2]=0;
				}
			glEnd();
		}
	glPopMatrix();
	glDepthMask (GL_TRUE);
    glDisable (GL_BLEND);
	//glCallList (sphereList);
    glutSwapBuffers();
}


void reshape(int w,int h)
{
        glViewport(0,0,(GLsizei) w, (GLsizei) h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(80.0,1.0,1.0,500.0);
        gluLookAt(eyex, eyey, eyez,lookx,looky,0,0.0,1.0,0.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
        int a;
        switch (key)
        {
			case '1':
			resolution=1;
			break;
			case '2':
			resolution=2;
			break;
			case '3':
			resolution=3;
			break;
			case '4':
			resolution=4;
			break;
			case '5':
			resolution=5;
			break;
			case '6':
			resolution=6;
			break;
			case '7':
			resolution=7;
			break;
			case '8':
			resolution=8;
			break;
			case '9':
			resolution=9;
			break;
			case '0':
			resolution=10;
			break;
			case '=':
			if (tailsize>0.2)
				tailsize-=0.1;
			break;
			case '-':
			tailsize+=0.1;
			break;
			case 32:
			displace(disx,disy);
			break;
			case 'v':
			printout(1);
			break;
			case 'b':
			printout(2);
			break;
			case 'n':
			printout(3);
			break;
			case 'm':
			printout(4);
			break;
			case 'l':
			lightswitch=1;
			break;
			case 'k':
			lightswitch=0;
			light0_position[0]=-50.0;
			light0_position[1]=-50.0;
			light1_position[0]=-50.0;
			light1_position[1]=-50.0;
			break;
			case 'f':
			flatswitch=1;
			break;
			case 'g':
			flatswitch=0;
			break;
			case 'u':
			layerswitch=0;
			break;
			case 'i':
			layerswitch=1;
			break;
			case 'o':
			layerswitch=2;
			break;
			case 'p':
			layerswitch=3;
			break;
			case '[':
			layerswitch=4;
			break;
			case ']':
			layerswitch=5;
			break;
			case 'q':
			eyex=100;
			eyey=100;
			eyez=150;
			lookx=100;
			looky=100;
			break;
            case 27:
            ritedata();
            exit(0);
            break;
        }
}

void idle(void) {
	glutPostRedisplay();
}


void mouse(int button, int state, int x, int y)
{
		//set orbiting mode
	if (button == GLUT_LEFT_BUTTON)
	    {
			mouseViewpointControl = 1;		// 1 for orbiting mode
		//set zooming mode
 		}
	else if (button == GLUT_RIGHT_BUTTON)
		{
			mouseViewpointControl = 2;		// 2 for zooming mode
		}
	if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
    return;
	glutPostRedisplay();
}



void motion(int x, int y)
{
		// if in orbiting mode...
	if (mouseViewpointControl == 1) {
		if (motionx > x) {
			eyex -= 2.0;
		}

		if (motionx < x) {
			eyex += 2.0;
		}

		if (motiony < y) {
			eyey -= 2.0;
		}

		if (motiony > y) {
			eyey += 2.0;
		}

		//else if in zooming mode
	} else if(mouseViewpointControl == 2){
		if (motiony > y) {
			eyez += 2;
		}

		if (motiony < y) {
			eyez -= 2;
		}
	}


	 glutPostRedisplay();

	 motionx = x;
	 motiony = y;

}


int main(int argc, char** argv)
{
        glutInit(&argc,argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
        glutInitWindowSize(700,700);
        glutInitWindowPosition(100,100);
        glutCreateWindow("wysiwya");
        init();
        glutDisplayFunc(display);
        glutReshapeFunc(reshape);
        glutKeyboardFunc(keyboard);
		glutMouseFunc(mouse);
		glutMotionFunc(motion);
		glutDisplayFunc(display);
		glutIdleFunc(idle);
        glutMainLoop();
        return 0;
}

void displace(int a,int b)
{
	int c,d;
	if (ground[a][b][1]==0)
	{
		if (dismax>=0)
			{
				if (a<matrixsize&&b<=matrixsize&&b>0&&a>0)
					{
						ground[a][b][0]=ground[a][b][0]+dismax;
						for (c=1;c<dismax;c++)
							{
								if ((a+c)<matrixsize)
									ground[a+c][b][0]=ground[a+c][b][0]+(dismax-c);
								if ((a-c)<matrixsize)
									ground[a-c][b][0]=ground[a-c][b][0]+(dismax-c);
								if ((b+c)<=matrixsize&&(b+c)>0)
									ground[a][b+c][0]=ground[a][b+c][0]+(dismax-c);
								if ((b-c)<=matrixsize&&(b-c)>0)
									ground[a][b-c][0]=ground[a][b-c][0]+(dismax-c);
								for (d=1;d<(dismax-c);d++)
									{
										if ((a+d)<matrixsize&&(b+c)<=matrixsize&&(b+c)>0&&(a+d)>0)
											ground[a+d][b+c][0]=ground[a+d][b+c][0]+((dismax-c)-d);
										if ((a-d)<matrixsize&&(b+c)<=matrixsize&&(b+c)>0&&(a-d)>0)
											ground[a-d][b+c][0]=ground[a-d][b+c][0]+((dismax-c)-d);
										if ((a+d)<matrixsize&&(b-c)<=matrixsize&&(b-c)>0&&(a+d)>0)
											ground[a+d][b-c][0]=ground[a+d][b-c][0]+((dismax-c)-d);
										if ((a-d)<matrixsize&&(b-c)<=matrixsize&&(b-c)>0&&(a-d)>0)
											ground[a-d][b-c][0]=ground[a-d][b-c][0]+((dismax-c)-d);
									}
							}
					}
			}
		if (dismax<0)
			{
				if (a<matrixsize&&b<=matrixsize&&b>0&&a>0)
					{
						ground[a][b][0]=ground[a][b][0]+dismax;
						for (c=dismax;c<0;c++)
							{
								if ((a+c)<matrixsize)
									ground[a+c][b][0]=ground[a+c][b][0]+(dismax-c);
								if ((a-c)<matrixsize)
									ground[a-c][b][0]=ground[a-c][b][0]+(dismax-c);
								if ((b+c)<=matrixsize&&(b+c)>0)
									ground[a][b+c][0]=ground[a][b+c][0]+(dismax-c);
								if ((b-c)<=matrixsize&&(b-c)>0)
									ground[a][b-c][0]=ground[a][b-c][0]+(dismax-c);
								for (d=(dismax-c);d<0;d++)
									{
										if ((a+d)<matrixsize&&(b+c)<=matrixsize&&(b+c)>0&&(a+d)>0)
											ground[a+d][b+c][0]=ground[a+d][b+c][0]+((dismax-c)-d);
										if ((a-d)<matrixsize&&(b+c)<=matrixsize&&(b+c)>0&&(a-d)>0)
											ground[a-d][b+c][0]=ground[a-d][b+c][0]+((dismax-c)-d);
										if ((a+d)<matrixsize&&(b-c)<=matrixsize&&(b-c)>0&&(a+d)>0)
											ground[a+d][b-c][0]=ground[a+d][b-c][0]+((dismax-c)-d);
										if ((a-d)<matrixsize&&(b-c)<=matrixsize&&(b-c)>0&&(a-d)>0)
											ground[a-d][b-c][0]=ground[a-d][b-c][0]+((dismax-c)-d);
									}
							}
					}
			}
	}
	ground[a][b][1]=1;
	glutPostRedisplay();
}

void comdisplace(int a,int b)
{
	int c,d;
	if (composite[a][b][1]==0)
	{
		if (comdismax>=0)
			{
				if (a<matrixsize&&b<=matrixsize&&b>0&&a>0)
					{
						composite[a][b][0]=composite[a][b][0]+comdismax;
						for (c=1;c<comdismax;c++)
							{
								if ((a+c)<matrixsize)
									composite[a+c][b][0]=composite[a+c][b][0]+(comdismax-c);
								if ((a-c)<matrixsize)
									composite[a-c][b][0]=composite[a-c][b][0]+(comdismax-c);
								if ((b+c)<=matrixsize&&(b+c)>0)
									composite[a][b+c][0]=composite[a][b+c][0]+(comdismax-c);
								if ((b-c)<=matrixsize&&(b-c)>0)
									composite[a][b-c][0]=composite[a][b-c][0]+(comdismax-c);
								for (d=1;d<(comdismax-c);d++)
									{
										if ((a+d)<matrixsize&&(b+c)<=matrixsize&&(b+c)>0&&(a+d)>0)
											composite[a+d][b+c][0]=composite[a+d][b+c][0]+((comdismax-c)-d);
										if ((a-d)<matrixsize&&(b+c)<=matrixsize&&(b+c)>0&&(a-d)>0)
											composite[a-d][b+c][0]=composite[a-d][b+c][0]+((comdismax-c)-d);
										if ((a+d)<matrixsize&&(b-c)<=matrixsize&&(b-c)>0&&(a+d)>0)
											composite[a+d][b-c][0]=composite[a+d][b-c][0]+((comdismax-c)-d);
										if ((a-d)<matrixsize&&(b-c)<=matrixsize&&(b-c)>0&&(a-d)>0)
											composite[a-d][b-c][0]=composite[a-d][b-c][0]+((comdismax-c)-d);
									}
							}
					}
			}
		if (comdismax<0)
			{
				if (a<matrixsize&&b<=matrixsize&&b>0&&a>0)
					{
						composite[a][b][0]=composite[a][b][0]+comdismax;
						for (c=comdismax;c<0;c++)
							{
								if ((a+c)<matrixsize)
									composite[a+c][b][0]=composite[a+c][b][0]+(comdismax-c);
								if ((a-c)<matrixsize)
									composite[a-c][b][0]=composite[a-c][b][0]+(comdismax-c);
								if ((b+c)<=matrixsize&&(b+c)>0)
									composite[a][b+c][0]=composite[a][b+c][0]+(comdismax-c);
								if ((b-c)<=matrixsize&&(b-c)>0)
									composite[a][b-c][0]=composite[a][b-c][0]+(comdismax-c);
								for (d=(comdismax-c);d<0;d++)
									{
										if ((a+d)<matrixsize&&(b+c)<=matrixsize&&(b+c)>0&&(a+d)>0)
											composite[a+d][b+c][0]=composite[a+d][b+c][0]+((comdismax-c)-d);
										if ((a-d)<matrixsize&&(b+c)<=matrixsize&&(b+c)>0&&(a-d)>0)
											composite[a-d][b+c][0]=composite[a-d][b+c][0]+((comdismax-c)-d);
										if ((a+d)<matrixsize&&(b-c)<=matrixsize&&(b-c)>0&&(a+d)>0)
											composite[a+d][b-c][0]=composite[a+d][b-c][0]+((comdismax-c)-d);
										if ((a-d)<matrixsize&&(b-c)<=matrixsize&&(b-c)>0&&(a-d)>0)
											composite[a-d][b-c][0]=composite[a-d][b-c][0]+((comdismax-c)-d);
									}
							}
					}
			}
	}
	composite[a][b][1]=1;
	glutPostRedisplay();
}

void textreader(void)
{
	int a,b,c,d,e,f,loadswitch=0,textcheck=0;
	char words[2];
	for(a=0;a<=matrixsize;a=a+1)
		{
			for(b=0;b<=matrixsize;b++)
				{
					words[0]=getc(environfp);
					if (words[0]=='a'||words[0]=='A')
						environmatrix[b][a][0]=1;
					if (words[0]=='b'||words[0]=='B')
						environmatrix[b][a][0]=2;
					if (words[0]=='c'||words[0]=='C')
						environmatrix[b][a][0]=3;
					if (words[0]=='d'||words[0]=='D')
						environmatrix[b][a][0]=4;
					if (words[0]=='e'||words[0]=='E')
						environmatrix[b][a][0]=5;
					if (words[0]=='f'||words[0]=='F')
						environmatrix[b][a][0]=6;
					if (words[0]=='g'||words[0]=='G')
						environmatrix[b][a][0]=7;
					if (words[0]=='h'||words[0]=='H')
						environmatrix[b][a][0]=8;
					if (words[0]=='i'||words[0]=='I')
						environmatrix[b][a][0]=9;
					if (words[0]=='j'||words[0]=='J')
						environmatrix[b][a][0]=10;
					if (words[0]=='k'||words[0]=='K')
						environmatrix[b][a][0]=11;
					if (words[0]=='l'||words[0]=='L')
						environmatrix[b][a][0]=12;
					if (words[0]=='m'||words[0]=='M')
						environmatrix[b][a][0]=13;
					if (words[0]=='n'||words[0]=='N')
						environmatrix[b][a][0]=14;
					if (words[0]=='o'||words[0]=='O')
						environmatrix[b][a][0]=15;
					if (words[0]=='p'||words[0]=='P')
						environmatrix[b][a][0]=16;
					if (words[0]=='q'||words[0]=='Q')
						environmatrix[b][a][0]=17;
					if (words[0]=='r'||words[0]=='R')
						environmatrix[b][a][0]=18;
					if (words[0]=='s'||words[0]=='S')
						environmatrix[b][a][0]=19;
					if (words[0]=='t'||words[0]=='T')
						environmatrix[b][a][0]=20;
					if (words[0]=='u'||words[0]=='U')
						environmatrix[b][a][0]=21;
					if (words[0]=='v'||words[0]=='V')
						environmatrix[b][a][0]=22;
					if (words[0]=='w'||words[0]=='W')
						environmatrix[b][a][0]=23;
					if (words[0]=='x'||words[0]=='X')
						environmatrix[b][a][0]=24;
					if (words[0]=='y'||words[0]=='Y')
						environmatrix[b][a][0]=25;
					if (words[0]=='z'||words[0]=='Z')
						environmatrix[b][a][0]=26;
					if (words[0]==EOF)
						{
							printf("Finished building environment matrix\n");
							a=matrixsize;
							b=matrixsize;
							loadswitch=1;
						}
					if (words[0]==EOF&&loadswitch==0)
						{
							printf("Finished building environment matrix\n");
							b=matrixsize;
							a=matrixsize;
							loadswitch=1;
						}
				}
		}
	if (loadswitch==0)
		printf("Finished building environment matrix\n");
	loadswitch=0;
	for(c=0;c<=matrixsize;c=c+1)
		{
			for(d=0;d<=matrixsize;d++)
				{
					words[0]=getc(system1fp);
					if (words[0]=='a'||words[0]=='A')
						system1matrix[d][c][0]=1;
					if (words[0]=='b'||words[0]=='B')
						system1matrix[d][c][0]=2;
					if (words[0]=='c'||words[0]=='C')
						system1matrix[d][c][0]=3;
					if (words[0]=='d'||words[0]=='D')
						system1matrix[d][c][0]=4;
					if (words[0]=='e'||words[0]=='E')
						system1matrix[d][c][0]=5;
					if (words[0]=='f'||words[0]=='F')
						system1matrix[d][c][0]=6;
					if (words[0]=='g'||words[0]=='G')
						system1matrix[d][c][0]=7;
					if (words[0]=='h'||words[0]=='H')
						system1matrix[d][c][0]=8;
					if (words[0]=='i'||words[0]=='I')
						system1matrix[d][c][0]=9;
					if (words[0]=='j'||words[0]=='J')
						system1matrix[d][c][0]=10;
					if (words[0]=='k'||words[0]=='K')
						system1matrix[d][c][0]=11;
					if (words[0]=='l'||words[0]=='L')
						system1matrix[d][c][0]=12;
					if (words[0]=='m'||words[0]=='M')
						system1matrix[d][c][0]=13;
					if (words[0]=='n'||words[0]=='N')
						system1matrix[d][c][0]=14;
					if (words[0]=='o'||words[0]=='O')
						system1matrix[d][c][0]=15;
					if (words[0]=='p'||words[0]=='P')
						system1matrix[d][c][0]=16;
					if (words[0]=='q'||words[0]=='Q')
						system1matrix[d][c][0]=17;
					if (words[0]=='r'||words[0]=='R')
						system1matrix[d][c][0]=18;
					if (words[0]=='s'||words[0]=='S')
						system1matrix[d][c][0]=19;
					if (words[0]=='t'||words[0]=='T')
						system1matrix[d][c][0]=20;
					if (words[0]=='u'||words[0]=='U')
						system1matrix[d][c][0]=21;
					if (words[0]=='v'||words[0]=='V')
						system1matrix[d][c][0]=22;
					if (words[0]=='w'||words[0]=='W')
						system1matrix[d][c][0]=23;
					if (words[0]=='x'||words[0]=='X')
						system1matrix[d][c][0]=24;
					if (words[0]=='y'||words[0]=='Y')
						system1matrix[d][c][0]=25;
					if (words[0]=='z'||words[0]=='Z')
						system1matrix[d][c][0]=26;
					if (words[0]==EOF)
						{
							printf("Finished building system #1 matrix\n");
							c=matrixsize;
							d=matrixsize;
							loadswitch=1;
						}
					if (words[0]==EOF&&loadswitch==0)
						{
							printf("Finished building system #1 matrix\n");
							d=matrixsize;
							c=matrixsize;
							loadswitch=1;
						}
				}
		}
	if (loadswitch==0)
		printf("Finished building system #1 matrix\n");
	loadswitch=0;
	for(e=0;e<=matrixsize;e=e+1)
		{
			for(f=0;f<=matrixsize;f++)
				{
					words[0]=getc(system2fp);
					if (words[0]=='a'||words[0]=='A')
						system2matrix[f][e][0]=1;
					if (words[0]=='b'||words[0]=='B')
						system2matrix[f][e][0]=2;
					if (words[0]=='c'||words[0]=='C')
						system2matrix[f][e][0]=3;
					if (words[0]=='d'||words[0]=='D')
						system2matrix[f][e][0]=4;
					if (words[0]=='e'||words[0]=='E')
						system2matrix[f][e][0]=5;
					if (words[0]=='f'||words[0]=='F')
						system2matrix[f][e][0]=6;
					if (words[0]=='g'||words[0]=='G')
						system2matrix[f][e][0]=7;
					if (words[0]=='h'||words[0]=='H')
						system2matrix[f][e][0]=8;
					if (words[0]=='i'||words[0]=='I')
						system2matrix[f][e][0]=9;
					if (words[0]=='j'||words[0]=='J')
						system2matrix[f][e][0]=10;
					if (words[0]=='k'||words[0]=='K')
						system2matrix[f][e][0]=11;
					if (words[0]=='l'||words[0]=='L')
						system2matrix[f][e][0]=12;
					if (words[0]=='m'||words[0]=='M')
						system2matrix[f][e][0]=13;
					if (words[0]=='n'||words[0]=='N')
						system2matrix[f][e][0]=14;
					if (words[0]=='o'||words[0]=='O')
						system2matrix[f][e][0]=15;
					if (words[0]=='p'||words[0]=='P')
						system2matrix[f][e][0]=16;
					if (words[0]=='q'||words[0]=='Q')
						system2matrix[f][e][0]=17;
					if (words[0]=='r'||words[0]=='R')
						system2matrix[f][e][0]=18;
					if (words[0]=='s'||words[0]=='S')
						system2matrix[f][e][0]=19;
					if (words[0]=='t'||words[0]=='T')
						system2matrix[f][e][0]=20;
					if (words[0]=='u'||words[0]=='U')
						system2matrix[f][e][0]=21;
					if (words[0]=='v'||words[0]=='V')
						system2matrix[f][e][0]=22;
					if (words[0]=='w'||words[0]=='W')
						system2matrix[f][e][0]=23;
					if (words[0]=='x'||words[0]=='X')
						system2matrix[f][e][0]=24;
					if (words[0]=='y'||words[0]=='Y')
						system2matrix[f][e][0]=25;
					if (words[0]=='z'||words[0]=='Z')
						system2matrix[f][e][0]=26;
					if (words[0]==EOF)
						{
							printf("Finished building system #2 matrix\n");
							e=matrixsize;
							f=matrixsize;
							loadswitch=1;
						}
					if (words[0]==EOF&&loadswitch==0)
						{
							printf("Finished building system #2 matrix\n");
							e=matrixsize;
							f=matrixsize;
							loadswitch=1;
						}
				}
		}
	if (loadswitch==0)
		printf("Finished building system #2 matrix\n");
}

void move(int a)
{
	if (a==1)
		{
			system1[0]=system1[0]+system1[3];
			system1[1]=system1[1]+system1[4];
			if (system1[3]!=0)
				system1[3]=system1[3]/sqrt(system1[3]*system1[3]);
			if (system1[4]!=0)
				system1[4]=system1[4]/sqrt(system1[4]*system1[4]);
			if ((environmatrix[system1[0]][system1[1]][0]-system1matrix[system1[0]][system1[1]][0])>0)
				{
					if (system1[3]+system1grav[system1[0]][system1[1]][0]!=system2[0]&&
						system1[4]-system1grav[system1[0]][system1[1]][1]!=system2[1])
						{
							if (system1[3]+system1grav[system1[0]][system1[1]][0]!=0||
								system1[4]-system1grav[system1[0]][system1[1]][1]!=0)
								{
									system1[3]=system1[3]+system1grav[system1[0]][system1[1]][0];
									system1[4]=system1[4]-system1grav[system1[0]][system1[1]][1];
								}
						}
				}
			else
				{
					if (system1[3]-system1grav[system1[0]][system1[1]][0]!=system2[0]&&
						system1[4]+system1grav[system1[0]][system1[1]][1]!=system2[1])
						{
							if (system1[3]-system1grav[system1[0]][system1[1]][0]!=0||
								system1[4]+system1grav[system1[0]][system1[1]][1]!=0)
								{
									system1[3]=system1[3]-system1grav[system1[0]][system1[1]][0];
									system1[4]=system1[4]+system1grav[system1[0]][system1[1]][1];
								}
						}
				}

			if (system1[0]>matrixsize||system1[0]<0)
				system1[3]=(-(system1[3]));
			if (system1[1]>matrixsize||system1[1]<0)
				system1[4]=(-(system1[4]));
			if (system1[0]>=matrixsize+1)
                system1[0]=matrixsize-1;
            if (system1[0]<=-1)
                system1[0]=1;
            if (system1[1]>=matrixsize+1)
                system1[1]=matrixsize-1;
            if (system1[1]<=-1)
                system1[1]=1;

		}
	if (a==2)
		{
			system2[0]=system2[0]+system2[3];
			system2[1]=system2[1]+system2[4];
			if (system2[3]!=0)
				system2[3]=system2[3]/sqrt(system2[3]*system2[3]);
			if (system2[4]!=0)
				system2[4]=system2[4]/sqrt(system2[4]*system2[4]);
			if ((environmatrix[system2[0]][system2[1]][0]-system2matrix[system2[0]][system2[1]][0])>0)
				{
					if (system2[3]-system1grav[system2[0]][system2[1]][0]!=system1[0]&&
						system2[4]+system1grav[system2[0]][system2[1]][1]!=system1[1])
						{
							if (system2[3]-system1grav[system2[0]][system2[1]][0]!=0||
								system2[4]+system1grav[system2[0]][system2[1]][1]!=0)
								{
									system2[3]=system2[3]-system1grav[system2[0]][system2[1]][0];
									system2[4]=system2[4]+system1grav[system2[0]][system2[1]][1];
								}
						}
				}
			else
				{
					if (system2[3]+system1grav[system2[0]][system2[1]][0]!=system1[0]&&
						system2[4]-system1grav[system2[0]][system2[1]][1]!=system1[1])
						{
							if (system2[3]+system1grav[system2[0]][system2[1]][0]!=0||
								system2[4]-system1grav[system2[0]][system2[1]][1]!=0)
								{
									system2[3]=system2[3]+system1grav[system2[0]][system2[1]][0];
									system2[4]=system2[4]-system1grav[system2[0]][system2[1]][1];
								}
						}
				}
			if (system2[0]>matrixsize||system2[0]<0)
				system2[3]=(-(system2[3]));
			if (system2[1]>matrixsize||system2[1]<0)
				system2[4]=(-(system2[4]));
			if (system2[0]>=matrixsize+1)
                system2[0]=matrixsize-1;
            if (system2[0]<=-1)
                system2[0]=1;
            if (system2[1]>=matrixsize+1)
                system2[1]=matrixsize-1;
       	    if (system2[1]<=-1)
	            system2[1]=1;
		}
}

void interpret(int a,int areax,int areay,int areax2, int areay2)
{
	int b,c;
	for(b=areay;b<=areay2;b=b+1)
		{
			for(c=areax;c<=areax2;c=c+1)
				{
					if (a==1)
						dismax=(environmatrix[c][b][0]-system1matrix[c][b][0])/sizeretard;
					if (a==2)
						dismax=(environmatrix[c][b][0]-system2matrix[c][b][0])/sizeretard;
					spot0cone=(environmatrix[c][b][0]-system1matrix[c][b][0])+50;
					spot1cone=(environmatrix[c][b][0]-system2matrix[c][b][0])+50;
					displace (c,b);
				}
		}
}

void remember(int a,int b,int c)
{
	int e,ee;
	int tempdiv0,tempdiv1;
	if (a==1)
		{
			mem1count+=1;
			if (mem1count>mem1countmax)
				{
					for (e=mem1countmax;e>1;e--)
						{
							system1mem[e-1][0]=system1mem[e][0];
							system1mem[e-1][1]=system1mem[e][1];
							system1mem[e-1][2]=system1mem[e][2];
							system1mem[e-1][3]=system1mem[e][3];
						}
					mem1count=mem1countmax;
					system1mem[mem1count][0]=b;
					system1mem[mem1count][1]=c;
					system1mem[mem1count][2]=(environmatrix[b][c][0]-system1matrix[b][c][0]);
					system1mem[mem1count][3]=system1matrix[b][c][0];
				}
			else
				{
					system1mem[mem1count][0]=b;
					system1mem[mem1count][1]=c;
					system1mem[mem1count][2]=(environmatrix[b][c][0]-system1matrix[b][c][0]);
					system1mem[mem1count][3]=system1matrix[b][c][0];
				}
			if (mem1count>=mem1countmax)
				{
					for (ee=2;ee<=mem1count;ee++)
						system1grav[b][c][0]=system1grav[b][c][0]+sqrt((system1mem[ee][2]-system1mem[ee-1][2])*
						(system1mem[ee][2]-system1mem[ee-1][2]));
					system1grav[b][c][0]=system1grav[b][c][0]/mem1count;
					system1mem[mem1count][4]=system1grav[b][c][0];
					tempdiv1=(system1mem[mem1count][1]-system1mem[mem1count-1][1]);
					tempdiv0=(system1mem[mem1count][0]-system1mem[mem1count-1][0]);
					if (tempdiv1==0)
						tempdiv1=1;
					if (tempdiv0==0)
						tempdiv0=1;
					system1grav[b][c][1]=system1grav[b][c][0]*
					(tempdiv1/sqrt(tempdiv1*tempdiv1));
					system1grav[b][c][0]=system1grav[b][c][0]*
					(tempdiv0/sqrt(tempdiv0*tempdiv0));
				}
		}
	if (a==2)
		{
			mem2count+=1;
			if (mem2count>mem2countmax)
				{
					for (e=mem2countmax;e>1;e--)
						{
							system2mem[e-1][0]=system2mem[e][0];
							system2mem[e-1][1]=system2mem[e][1];
							system2mem[e-1][2]=system2mem[e][2];
							system2mem[e-1][3]=system2mem[e][3];
						}
					mem2count=mem2countmax;
					system2mem[mem2count][0]=b;
					system2mem[mem2count][1]=c;
					system2mem[mem2count][2]=(environmatrix[b][c][0]-system2matrix[b][c][0]);
					system2mem[mem2count][3]=system2matrix[b][c][0];
				}
			else
				{
					system2mem[mem2count][0]=b;
					system2mem[mem2count][1]=c;
					system2mem[mem2count][2]=(environmatrix[b][c][0]-system2matrix[b][c][0]);
					system2mem[mem2count][3]=system2matrix[b][c][0];
				}
			if (mem2count>=mem2countmax)
				{
					for (ee=2;ee<=mem2count;ee++)
						system1grav[b][c][0]=system1grav[b][c][0]+sqrt((system2mem[ee][2]-system2mem[ee-1][2])*
						(system2mem[ee][2]-system2mem[ee-1][2]));
					system1grav[b][c][0]=system1grav[b][c][0]/mem2count;
					system2mem[mem2count][4]=system1grav[b][c][0];
					tempdiv1=(system2mem[mem2count][1]-system2mem[mem2count-1][1]);
					tempdiv0=(system2mem[mem2count][0]-system2mem[mem2count-1][0]);
					if (tempdiv1==0)
						tempdiv1=1;
					if (tempdiv0==0)
						tempdiv0=1;
					system1grav[b][c][1]=system1grav[b][c][0]*
					(tempdiv1/sqrt(tempdiv1*tempdiv1));
					system1grav[b][c][0]=system1grav[b][c][0]*
					(tempdiv0/sqrt(tempdiv0*tempdiv0));
				}
		}
}

void compare(int a,int b,int c)
{
	if (system1matrix[b][c][0]==system2matrix[b][c][0])
		{
			if (composite[b][c][1]==1)
				{
					comdismax=(-((environmatrix[b][c][0]-composite2[b][c][0])/sizeretard));
					composite[b][c][1]=0;
					comdisplace(b,c);
					composite[b][c][1]=0;
				}
			composite2[b][c][0]=system1matrix[b][c][0];
			comdismax=(environmatrix[b][c][0]-composite2[b][c][0])/sizeretard;
			comdisplace(b,c);
		}
}

void communication(int a,int b,int c)
{
	int d;
	if (mem1count>=mem1countmax)
		{
		if (system1mem[mem1count][4]==system2mem[mem2count][4])
			{
			for (d=1;d<=mem1count;d++)
				{
					if (a==1)
						system1matrix[system2mem[d][0]][system2mem[d][1]][0]=system2mem[d][3];
					if (a==2)
						system2matrix[system1mem[d][0]][system1mem[d][1]][0]=system1mem[d][3];
				}
				if (a==1)
					{
						system1commax=system2mem[mem2count][3];
						system1comcount=0;
					}
				if (a==2)
					{
						system2commax=system1mem[mem1count][3];
						system2comcount=0;
					}
			}
		}
}

void rebound (void)
{
	int a,b;
	for(a=0;a<=matrixsize;a=a+1)
		{
			for(b=0;b<=matrixsize;b=b+1)
				{
					if (ground[b][a][0]>composite[b][a][0])
						ground[b][a][0]-=tailsize;
					if (ground[b][a][0]<composite[b][a][0])
						ground[b][a][0]+=tailsize;
					if (ground[b][a][0]==composite[b][a][0])
						ground[b][a][1]=0;
					if (ground[b][a][0]<composite[b][a][0]+tailsize&&ground[b][a][0]>composite[b][a][0]-tailsize)
						{
							ground[b][a][0]=composite[b][a][0];
							ground[b][a][1]=0;
						}
				}
		}
}

void printout(int a)
{
	int e,f;
	if (a==1)
		{
			for(e=0;e<=matrixsize;e=e+1)
				{
					for(f=0;f<=matrixsize;f++)
						{
							if (system1matrix[f][e][0]==0)
								printf(" ");
							if (system1matrix[f][e][0]==1)
								printf("a");
							if (system1matrix[f][e][0]==2)
								printf("b");
							if (system1matrix[f][e][0]==3)
								printf("c");
							if (system1matrix[f][e][0]==4)
								printf("d");
							if (system1matrix[f][e][0]==5)
								printf("e");
							if (system1matrix[f][e][0]==6)
								printf("f");
							if (system1matrix[f][e][0]==7)
								printf("g");
							if (system1matrix[f][e][0]==8)
								printf("h");
							if (system1matrix[f][e][0]==9)
								printf("i");
							if (system1matrix[f][e][0]==10)
								printf("j");
							if (system1matrix[f][e][0]==11)
								printf("k");
							if (system1matrix[f][e][0]==12)
								printf("l");
							if (system1matrix[f][e][0]==13)
								printf("m");
							if (system1matrix[f][e][0]==14)
								printf("n");
							if (system1matrix[f][e][0]==15)
								printf("o");
							if (system1matrix[f][e][0]==16)
								printf("p");
							if (system1matrix[f][e][0]==17)
								printf("q");
							if (system1matrix[f][e][0]==18)
								printf("r");
							if (system1matrix[f][e][0]==19)
								printf("s");
							if (system1matrix[f][e][0]==20)
								printf("t");
							if (system1matrix[f][e][0]==21)
								printf("u");
							if (system1matrix[f][e][0]==22)
								printf("v");
							if (system1matrix[f][e][0]==23)
								printf("w");
							if (system1matrix[f][e][0]==24)
								printf("x");
							if (system1matrix[f][e][0]==25)
								printf("y");
							if (system1matrix[f][e][0]==26)
								printf("z");
						}
				}
		}
	if (a==2)
		{
			for(e=0;e<=matrixsize;e=e+1)
				{
					for(f=0;f<=matrixsize;f++)
						{
							if (system2matrix[f][e][0]==0)
								printf(" ");
							if (system2matrix[f][e][0]==1)
								printf("a");
							if (system2matrix[f][e][0]==2)
								printf("b");
							if (system2matrix[f][e][0]==3)
								printf("c");
							if (system2matrix[f][e][0]==4)
								printf("d");
							if (system2matrix[f][e][0]==5)
								printf("e");
							if (system2matrix[f][e][0]==6)
								printf("f");
							if (system2matrix[f][e][0]==7)
								printf("g");
							if (system2matrix[f][e][0]==8)
								printf("h");
							if (system2matrix[f][e][0]==9)
								printf("i");
							if (system2matrix[f][e][0]==10)
								printf("j");
							if (system2matrix[f][e][0]==11)
								printf("k");
							if (system2matrix[f][e][0]==12)
								printf("l");
							if (system2matrix[f][e][0]==13)
								printf("m");
							if (system2matrix[f][e][0]==14)
								printf("n");
							if (system2matrix[f][e][0]==15)
								printf("o");
							if (system2matrix[f][e][0]==16)
								printf("p");
							if (system2matrix[f][e][0]==17)
								printf("q");
							if (system2matrix[f][e][0]==18)
								printf("r");
							if (system2matrix[f][e][0]==19)
								printf("s");
							if (system2matrix[f][e][0]==20)
								printf("t");
							if (system2matrix[f][e][0]==21)
								printf("u");
							if (system2matrix[f][e][0]==22)
								printf("v");
							if (system2matrix[f][e][0]==23)
								printf("w");
							if (system2matrix[f][e][0]==24)
								printf("x");
							if (system2matrix[f][e][0]==25)
								printf("y");
							if (system2matrix[f][e][0]==26)
								printf("z");
						}
				}
		}
	if (a==3)
		{
			for(e=0;e<=matrixsize;e=e+1)
				{
					for(f=0;f<=matrixsize;f++)
						{
							if (environmatrix[f][e][0]==0)
								printf(" ");
							if (environmatrix[f][e][0]==1)
								printf("a");
							if (environmatrix[f][e][0]==2)
								printf("b");
							if (environmatrix[f][e][0]==3)
								printf("c");
							if (environmatrix[f][e][0]==4)
								printf("d");
							if (environmatrix[f][e][0]==5)
								printf("e");
							if (environmatrix[f][e][0]==6)
								printf("f");
							if (environmatrix[f][e][0]==7)
								printf("g");
							if (environmatrix[f][e][0]==8)
								printf("h");
							if (environmatrix[f][e][0]==9)
								printf("i");
							if (environmatrix[f][e][0]==10)
								printf("j");
							if (environmatrix[f][e][0]==11)
								printf("k");
							if (environmatrix[f][e][0]==12)
								printf("l");
							if (environmatrix[f][e][0]==13)
								printf("m");
							if (environmatrix[f][e][0]==14)
								printf("n");
							if (environmatrix[f][e][0]==15)
								printf("o");
							if (environmatrix[f][e][0]==16)
								printf("p");
							if (environmatrix[f][e][0]==17)
								printf("q");
							if (environmatrix[f][e][0]==18)
								printf("r");
							if (environmatrix[f][e][0]==19)
								printf("s");
							if (environmatrix[f][e][0]==20)
								printf("t");
							if (environmatrix[f][e][0]==21)
								printf("u");
							if (environmatrix[f][e][0]==22)
								printf("v");
							if (environmatrix[f][e][0]==23)
								printf("w");
							if (environmatrix[f][e][0]==24)
								printf("x");
							if (environmatrix[f][e][0]==25)
								printf("y");
							if (environmatrix[f][e][0]==26)
								printf("z");
						}
				}
		}
	if (a==4)
		{
			for(e=0;e<=matrixsize;e=e+1)
				{
					for(f=0;f<=matrixsize;f++)
						{
							if (composite2[f][e][0]==0)
								printf(" ");
							if (composite2[f][e][0]==1)
								printf("a");
							if (composite2[f][e][0]==2)
								printf("b");
							if (composite2[f][e][0]==3)
								printf("c");
							if (composite2[f][e][0]==4)
								printf("d");
							if (composite2[f][e][0]==5)
								printf("e");
							if (composite2[f][e][0]==6)
								printf("f");
							if (composite2[f][e][0]==7)
								printf("g");
							if (composite2[f][e][0]==8)
								printf("h");
							if (composite2[f][e][0]==9)
								printf("i");
							if (composite2[f][e][0]==10)
								printf("j");
							if (composite2[f][e][0]==11)
								printf("k");
							if (composite2[f][e][0]==12)
								printf("l");
							if (composite2[f][e][0]==13)
								printf("m");
							if (composite2[f][e][0]==14)
								printf("n");
							if (composite2[f][e][0]==15)
								printf("o");
							if (composite2[f][e][0]==16)
								printf("p");
							if (composite2[f][e][0]==17)
								printf("q");
							if (composite2[f][e][0]==18)
								printf("r");
							if (composite2[f][e][0]==19)
								printf("s");
							if (composite2[f][e][0]==20)
								printf("t");
							if (composite2[f][e][0]==21)
								printf("u");
							if (composite2[f][e][0]==22)
								printf("v");
							if (composite2[f][e][0]==23)
								printf("w");
							if (composite2[f][e][0]==24)
								printf("x");
							if (composite2[f][e][0]==25)
								printf("y");
							if (composite2[f][e][0]==26)
								printf("z");
						}
				}
		}
}

void ritedata(void)
{
	fclose(environfp);
	fclose(system1fp);
	fclose(system2fp);
}
