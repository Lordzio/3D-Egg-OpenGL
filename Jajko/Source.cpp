/*************************************************************************************/

//  Szkielet programu do tworzenia modelu sceny 3-D z wizualizacj¹ osi 
//  uk³adu wspó³rzednych

/*************************************************************************************/

#include <windows.h>
#include <gl/gl.h>
#include "glut.h"
#include <math.h>
#include <time.h>
#include <iostream>
#include <vector>
#define M_PI 3.14159265358979323846

#pragma region Zmienne
typedef float point3[3];
const int N = 30;
static GLfloat viewer[] = { 0.0, 0.0, 10.0 };
int zoom = 10;
static GLfloat thetax = 0.0;   // k¹t obrotu obiektu
static GLfloat thetay = 0.0;
static GLfloat pix2angle;     // przelicznik pikseli na stopnie
static GLfloat pix2angle1;


GLfloat light_position0[] = { 0, 0, 10, 1.0 };
GLfloat light_position1[] = { 0, 0, 0, 1.0 };

static GLfloat x = 0;
static GLfloat y = 0;
static GLfloat z = 10;

static GLint statusl = 0;       // stan klawiszy myszy
static GLint statusr = 0;
// 0 - nie naciœniêto ¿adnego klawisza
// 1 - naciœniêty zostaæ lewy klawisz

static int x_pos_old = 0;       // poprzednia pozycja kursora myszy
static int y_pos_old = 0;

static int delta_x = 0;
static int delta_y = 0;
static int delta_ry = 0;
// ró¿nica pomiêdzy pozycj¹ bie¿¹c¹
// i poprzedni¹ kursora myszy


struct Point
{
	GLfloat x, y, z;
};
Point ColorArray[N + 1][N + 1] = { NULL };
Point TextArr[N + 1][N + 1] = { NULL };
#pragma endregion
/*************************************************************************************/
// Funkcja "bada" stan myszy i ustawia wartoœci odpowiednich zmiennych globalnych
#pragma region Mysz
void Mouse(int btn, int state, int x, int y)
{


	if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		x_pos_old = x;
		y_pos_old = y;// jako pozycji poprzedniej
		statusr = 1;          // wciêniêty zosta³ prawy klawisz myszy
	}
	else
		if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			x_pos_old = x;        // przypisanie aktualnie odczytanej pozycji kursora
			y_pos_old = y;// jako pozycji poprzedniej
			statusl = 1;          // wciêniêty zosta³ lewy klawisz myszy
		}
		else

			statusr = statusl = 0;// nie zosta³ wciêniêty ¿aden klawisz
}

/*************************************************************************************/
// Funkcja "monitoruje" po³o¿enie kursora myszy i ustawia wartoœci odpowiednich
// zmiennych globalnych

void Motion(GLsizei x, GLsizei y)
{

	delta_x = x - x_pos_old;     // obliczenie ró¿nicy po³o¿enia kursora myszy
	delta_y = y - y_pos_old;

	x_pos_old = x;            // podstawienie bie¿¹cego po³o¿enia jako poprzednie
	y_pos_old = y;

	glutPostRedisplay();     // przerysowanie obrazu sceny
}

/*************************************************************************************/

#pragma endregion



/*************************************************************************************/

// Funkcja rysuj¹ca osie uk³adu wspó³rzêdnych


int model = 2;  // 1- punkty, 2- siatka, 3 - wype³nione trójk¹ty
bool key1 = true, key2 = true, key3 = true, key4 = true, key5 = true, key6 = true;
void RenderScene();


void keys(unsigned char key, int x, int y)
{
	if (key == 'w') model = 1;
	if (key == 'p') model = 2;
	if (key == '1')
		if (key1) key1 = false;
		else key1 = true;
	if (key == '2')
		if (key2) key2 = false;
		else key2 = true;
	if (key == '3')
		if (key3) key3 = false;
		else key3 = true;
	if (key == '4')
		if (key4) key4 = false;
		else key4 = true;
	if (key == '5')
		if (key5) key5 = false;
		else key5 = true;

	if (key == '6')
		if (key6) {
			glDisable(GL_CULL_FACE);
			key6 = false;
		}
		else{
			glEnable(GL_CULL_FACE);
			key6 = true;
		}
	RenderScene(); // przerysowanie obrazu sceny
}

void Axes(void)
{

	point3  x_min = { -5.0, 0.0, 0.0 };
	point3  x_max = { 5.0, 0.0, 0.0 };
	// pocz¹tek i koniec obrazu osi x

	point3  y_min = { 0.0, -5.0, 0.0 };
	point3  y_max = { 0.0, 5.0, 0.0 };
	// pocz¹tek i koniec obrazu osi y

	point3  z_min = { 0.0, 0.0, -5.0 };
	point3  z_max = { 0.0, 0.0, 5.0 };
	//  pocz¹tek i koniec obrazu osi z

	glColor3f(1.0f, 0.0f, 0.0f);  // kolor rysowania osi - czerwony
	glBegin(GL_LINES); // rysowanie osi x

	glVertex3fv(x_min);
	glVertex3fv(x_max);

	glEnd();

	glColor3f(0.0f, 1.0f, 0.0f);  // kolor rysowania - zielony
	glBegin(GL_LINES);  // rysowanie osi y

	glVertex3fv(y_min);
	glVertex3fv(y_max);

	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);  // kolor rysowania - niebieski
	glBegin(GL_LINES); // rysowanie osi z

	glVertex3fv(z_min);
	glVertex3fv(z_max);

	glEnd();

}

/*************************************************************************************/
#pragma region Settery
GLfloat setX(int N, int i, int k)
{
	double u = (double)i / (double)N;
	double v = (double)k / (double)N;
	return ((-90 * pow(u, 5)) + (225 * pow(u, 4)) + (-270 * pow(u, 3)) + (180 * pow(u, 2) - 45 * u)) * cos(M_PI * v);
}

GLfloat setY(int N, int i, int k)
{
	double u = (double)i / (double)N;
	double v = (double)k / (double)N;
	return (160 * pow(u, 4)) + (-320 * pow(u, 3)) + (160 * pow(u, 2));
}

GLfloat setZ(int N, int i, int k)
{
	double u = (double)i / (double)N;
	double v = (double)k / (double)N;
	return ((-90 * pow(u, 5)) + (225 * pow(u, 4)) + (-270 * pow(u, 3)) + (180 * pow(u, 2)) + (-45 * u)) * sin(M_PI * v);
}
#pragma endregion

/*************************************************************************************/
// Funkcja wczytuje dane obrazu zapisanego w formacie TGA w pliku o nazwie
// FileName, alokuje pamiêæ i zwraca wskaŸnik (pBits) do bufora w którym
// umieszczone s¹ dane.
// Ponadto udostêpnia szerokoœæ (ImWidth), wysokoœæ (ImHeight) obrazu
// tekstury oraz dane opisuj¹ce format obrazu wed³ug specyfikacji OpenGL
// (ImComponents) i (ImFormat).
// Jest to bardzo uproszczona wersja funkcji wczytuj¹cej dane z pliku TGA.
// Dzia³a tylko dla obrazów wykorzystuj¹cych 8, 24, or 32 bitowy kolor.
// Nie obs³uguje plików w formacie TGA kodowanych z kompresj¹ RLE.
/*************************************************************************************/


GLbyte *LoadTGAImage(const char *FileName, GLint *ImWidth, GLint *ImHeight, GLint *ImComponents, GLenum *ImFormat)
{

	/*************************************************************************************/

	// Struktura dla nag³ówka pliku  TGA


#pragma pack(1)           
	typedef struct
	{
		GLbyte    idlength;
		GLbyte    colormaptype;
		GLbyte    datatypecode;
		unsigned short    colormapstart;
		unsigned short    colormaplength;
		unsigned char     colormapdepth;
		unsigned short    x_orgin;
		unsigned short    y_orgin;
		unsigned short    width;
		unsigned short    height;
		GLbyte    bitsperpixel;
		GLbyte    descriptor;
	}TGAHEADER;
#pragma pack(8)

	FILE *pFile;
	TGAHEADER tgaHeader;
	unsigned long lImageSize;
	short sDepth;
	GLbyte    *pbitsperpixel = NULL;


	/*************************************************************************************/

	// Wartoœci domyœlne zwracane w przypadku b³êdu

	*ImWidth = 0;
	*ImHeight = 0;
	*ImFormat = GL_BGR_EXT;
	*ImComponents = GL_RGB8;

	pFile = fopen(FileName, "rb");
	if (pFile == NULL)
		return NULL;

	/*************************************************************************************/
	// Przeczytanie nag³ówka pliku 


	fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);


	/*************************************************************************************/

	// Odczytanie szerokoœci, wysokoœci i g³êbi obrazu

	*ImWidth = tgaHeader.width;
	*ImHeight = tgaHeader.height;
	sDepth = tgaHeader.bitsperpixel / 8;


	/*************************************************************************************/
	// Sprawdzenie, czy g³êbia spe³nia za³o¿one warunki (8, 24, lub 32 bity)

	if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32)
		return NULL;

	/*************************************************************************************/

	// Obliczenie rozmiaru bufora w pamiêci


	lImageSize = tgaHeader.width * tgaHeader.height * sDepth;


	/*************************************************************************************/

	// Alokacja pamiêci dla danych obrazu


	pbitsperpixel = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));

	if (pbitsperpixel == NULL)
		return NULL;

	if (fread(pbitsperpixel, lImageSize, 1, pFile) != 1)
	{
		free(pbitsperpixel);
		return NULL;
	}


	/*************************************************************************************/

	// Ustawienie formatu OpenGL


	switch (sDepth)

	{

	case 3:

		*ImFormat = GL_BGR_EXT;

		*ImComponents = GL_RGB8;

		break;

	case 4:

		*ImFormat = GL_BGRA_EXT;

		*ImComponents = GL_RGBA8;

		break;

	case 1:

		*ImFormat = GL_LUMINANCE;

		*ImComponents = GL_LUMINANCE8;

		break;

	};



	fclose(pFile);



	return pbitsperpixel;

}

/*************************************************************************************/


void DrawTriangles(Point PointArray[N + 1][N + 1], GLfloat extraArrayJustBecauseWhyNot[N + 1][N + 1][3], GLfloat extraArrayJustBecauseWhyNotInv[N + 1][N + 1][3], int start, int stop) {
	GLfloat *v = NULL;

	

	if (start <= stop){
		for (int i = start, j = 0; i < stop; i++, j++) {
			for (int k = 0; k < N; k++) {

				v = extraArrayJustBecauseWhyNot[i][k];
				glNormal3fv(v);
				glTexCoord2f(TextArr[i][k].x, TextArr[i][k].y);
				glVertex3f(PointArray[i][k].x, PointArray[i][k].y - 5, PointArray[i][k].z);

				v = extraArrayJustBecauseWhyNot[i][k + 1];
				glNormal3fv(v);
				glTexCoord2f(TextArr[i][k + 1].x, TextArr[i][k + 1].y);
				glVertex3f(PointArray[i][k + 1].x, PointArray[i][k + 1].y - 5, PointArray[i][k + 1].z);

				v = extraArrayJustBecauseWhyNot[i + 1][k + 1];
				glNormal3fv(v);
				glTexCoord2f(TextArr[i + 1][k + 1].x, TextArr[i + 1][k + 1].y);
				glVertex3f(PointArray[i + 1][k + 1].x, PointArray[i + 1][k + 1].y - 5, PointArray[i + 1][k + 1].z);
				///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				v = extraArrayJustBecauseWhyNot[i + 1][k + 1];
				glNormal3fv(v);
				glTexCoord2f(TextArr[i + 1][k + 1].x, TextArr[i + 1][k + 1].y);
				glVertex3f(PointArray[i + 1][k + 1].x, PointArray[i + 1][k + 1].y - 5, PointArray[i + 1][k + 1].z);

				v = extraArrayJustBecauseWhyNot[i + 1][k];
				glNormal3fv(v);
				glTexCoord2f(TextArr[i + 1][k].x, TextArr[i + 1][k].y);
				glVertex3f(PointArray[i + 1][k].x, PointArray[i + 1][k].y - 5, PointArray[i + 1][k].z);

				v = extraArrayJustBecauseWhyNot[i][k];
				glNormal3fv(v);
				glTexCoord2f(TextArr[i][k].x, TextArr[i][k].y);
				glVertex3f(PointArray[i][k].x, PointArray[i][k].y - 5, PointArray[i][k].z);
				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			}
		}
	}
 else{
	 int buf = start;
	 start = stop;
	 stop = buf;
for (int i = start, j = 0; i < stop; i++, j++) {
			for (int k = 0; k < N; k++) {
				v = extraArrayJustBecauseWhyNot[i + 1][k + 1];
				glNormal3fv(v);
				glTexCoord2f(TextArr[i + 1][k + 1].x, TextArr[i + 1][k + 1].y);
				glVertex3f(PointArray[i + 1][k + 1].x, PointArray[i + 1][k + 1].y - 5, PointArray[i + 1][k + 1].z);

				v = extraArrayJustBecauseWhyNot[i][k + 1];
				glNormal3fv(v);
				glTexCoord2f(TextArr[i][k + 1].x, TextArr[i][k + 1].y);
				glVertex3f(PointArray[i][k + 1].x, PointArray[i][k + 1].y - 5, PointArray[i][k + 1].z);

				v = extraArrayJustBecauseWhyNot[i][k];
				glNormal3fv(v);
				glTexCoord2f(TextArr[i][k].x, TextArr[i][k].y);
				glVertex3f(PointArray[i][k].x, PointArray[i][k].y - 5, PointArray[i][k].z);
				///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				v = extraArrayJustBecauseWhyNot[i][k];
				glNormal3fv(v);
				glTexCoord2f(TextArr[i][k].x, TextArr[i][k].y);
				glVertex3f(PointArray[i][k].x, PointArray[i][k].y - 5, PointArray[i][k].z);

				v = extraArrayJustBecauseWhyNot[i + 1][k];
				glNormal3fv(v);
				glTexCoord2f(TextArr[i + 1][k].x, TextArr[i + 1][k].y);
				glVertex3f(PointArray[i + 1][k].x, PointArray[i + 1][k].y - 5, PointArray[i + 1][k].z);

				v = extraArrayJustBecauseWhyNot[i + 1][k + 1];
				glNormal3fv(v);
				glTexCoord2f(TextArr[i + 1][k + 1].x, TextArr[i + 1][k + 1].y);
				glVertex3f(PointArray[i + 1][k + 1].x, PointArray[i + 1][k + 1].y - 5, PointArray[i + 1][k + 1].z);
				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			}
		}
 }
}

void DrawPyramid(){

	glBegin(GL_TRIANGLES);
	if (key1){
		glNormal3f(0, 0, 1);
		glTexCoord2f(0.5, 1.0);
		glVertex3f(-2.5, -1.25, -2.5);

		glNormal3f(0, 0, 1);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(0, 1.25, 0);

		glNormal3f(0, 0, 1);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(2.5, -1.25, -2.5);
	}

	if (key2){
		glNormal3f(0, 0, -1);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(2.5, -1.25, 2.5);

		glNormal3f(0, 0, -1);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(0, 1.25, 0);

		glNormal3f(0, 0, -1);
		glTexCoord2f(0.5, 1.0);
		glVertex3f(-2.5, -1.25, 2.5);
	}

	if (key3){
		glNormal3f(-1, 0, 0);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-2.5, -1.25, 2.5);

		glNormal3f(-1, 0, 0);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(0, 1.25, 0);

		glNormal3f(-1, 0, 0);
		glTexCoord2f(0.5, 1.0);
		glVertex3f(-2.5, -1.25, -2.5);
	}

	if (key4){
		glNormal3f(1, 0, 0);
		glTexCoord2f(0.5, 1.0);
		glVertex3f(2.5, -1.25, -2.5);

		glNormal3f(1, 0, 0);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(0, 1.25, 0);

		glNormal3f(1, 0, 0);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(2.5, -1.25, 2.5);
	}
	glEnd();

	glBegin(GL_QUADS);
	if (key5){
		glNormal3f(0, -1, 0);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-2.5, -1.25, -2.5);

		glNormal3f(0, -1, 0);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(2.5, -1.25, -2.5);

		glNormal3f(0, -1, 0);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(2.5, -1.25, 2.5);

		glNormal3f(0, -1, 0);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-2.5, -1.25, 2.5);
	}
	glEnd();
}




// Funkcja okreœlaj¹ca co ma byæ rysowane (zawsze wywo³ywana gdy trzeba
// przerysowaæ scenê)
void Drawing()
{

	Point PointArray[N + 1][N + 1] = { NULL };
	Point norm[N + 1][N + 1] = { NULL };

	GLfloat extraArrayJustBecauseWhyNot1[N + 1][N + 1][3];
	GLfloat extraArrayJustBecauseWhyNot2[N + 1][N + 1][3];
	for (int i = 0; i < N + 1; i++)
	{
		for (int k = 0; k < N + 1; k++)
		{
			PointArray[i][k].x = setX(N, i, k);
			PointArray[i][k].y = setY(N, i, k);
			PointArray[i][k].z = setZ(N, i, k);

			double u = (double)i / (double)N;
			double v = (double)k / (double)N;

			TextArr[i][k].x = v;
			TextArr[i][k].y = u;

			GLfloat xu = ((-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * cos(M_PI * v));
			GLfloat	xv = ((90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * M_PI * sin(M_PI * v));
			GLfloat yu = 640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u;
			GLfloat yv = 0;
			GLfloat zu = ((-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45)*sin(M_PI * v));
			GLfloat zv = -1 * M_PI * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * cos(M_PI * v);
			norm[i][k].x = yu * zv - zu * yv;
			norm[i][k].y = zu * xv - xu * zv;
			norm[i][k].z = xu * yv - yu * xv;
			double ln = sqrt(pow(norm[i][k].x, 2) + pow(norm[i][k].y, 2) + pow(norm[i][k].z, 2));
			norm[i][k].x /= ln;
			norm[i][k].y /= ln;
			norm[i][k].z /= ln;

			extraArrayJustBecauseWhyNot1[i][k][0] = norm[i][k].x;
			extraArrayJustBecauseWhyNot1[i][k][1] = norm[i][k].y;
			extraArrayJustBecauseWhyNot1[i][k][2] = norm[i][k].z;
			extraArrayJustBecauseWhyNot2[i][k][0] = -norm[i][k].x;
			extraArrayJustBecauseWhyNot2[i][k][1] = -norm[i][k].y;
			extraArrayJustBecauseWhyNot2[i][k][2] = -norm[i][k].z;
		};
	}

	for (int i = 0; i < N; i++) {
		extraArrayJustBecauseWhyNot1[0][i][0] = 0;
		extraArrayJustBecauseWhyNot1[0][i][1] = -1;
		extraArrayJustBecauseWhyNot1[0][i][2] = 0;

		extraArrayJustBecauseWhyNot1[N / 2][i][0] = 0;
		extraArrayJustBecauseWhyNot1[N / 2][i][1] = 1;
		extraArrayJustBecauseWhyNot1[N / 2][i][2] = 0;

		extraArrayJustBecauseWhyNot2[N][i][0] = 0;
		extraArrayJustBecauseWhyNot2[N][i][1] = -1;
		extraArrayJustBecauseWhyNot2[N][i][2] = 0;

		extraArrayJustBecauseWhyNot2[N / 2][i][0] = 0;
		extraArrayJustBecauseWhyNot2[N / 2][i][1] = 1;
		extraArrayJustBecauseWhyNot2[N / 2][i][2] = 0;
	}

	switch (model)
	{

	case 1:
		glBegin(GL_TRIANGLES);
		
		DrawTriangles(PointArray, extraArrayJustBecauseWhyNot1, extraArrayJustBecauseWhyNot2, N / 2, 0);
		DrawTriangles(PointArray, extraArrayJustBecauseWhyNot2, extraArrayJustBecauseWhyNot1, N / 2, N);


		glEnd();
		break;

	case 2:
		DrawPyramid();
		break;
	default: break;
	}
}


void RenderScene(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszcz¹cym

	glLoadIdentity();
	// Czyszczenie macierzy bie¿¹cej


	// Narysowanie osi przy pomocy funkcji zdefiniowanej wy¿ej
	if (statusl == 1)                     // jeœli lewy klawisz myszy wciêniêty
	{
		if (delta_x) thetax += delta_x*pix2angle*0.01;
		// modyfikacja k¹ta obrotu o kat proporcjonalny
		if (delta_y) thetay += delta_y*pix2angle1*0.01;	// do ró¿nicy po³o¿eñ kursora myszy

														//if (thetax < - 2 * M_PI) thetax = 0;
														//if (thetax > 2 * M_PI) thetax = 0;
														//if (thetay < - 2 * M_PI) thetay = 0;
														//if (thetay > 2 * M_PI) thetay = 0;

	}
	if (statusr == 1) {
		if (delta_y > 0) zoom++;
		if (delta_y < 0) zoom--;
		if (zoom < 7) zoom = 7;
		if (zoom > 27) zoom = 27;

		//if (delta_x) thetaxr+= delta_x*pix2angle*0.01;
		//// modyfikacja k¹ta obrotu o kat proporcjonalny
		//if (delta_y) thetayr += delta_y*pix2angle1*0.01;

	}


	x = zoom*cos(thetax)*cos(thetay);
	z = zoom*sin(thetax)*cos(thetay);
	y = zoom*sin(thetay);
	//if (statusl == 1){
	//	light_position0[0] = -x;
	//	light_position0[1] = -y;
	//	light_position0[2] = z;
	//	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	//}
	//if (statusr == 1){
	//	light_position1[0] = -x;
	//	light_position1[1] = -y;
	//	light_position1[2] = z;
	//	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
	//}
//	gluLookAt(viewer[0], viewer[1], zoom, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, cos(thetay), 0.0);

	//glRotatef(thetax, 0.0, 1.0, 0.0);  //obrót obiektu o nowy k¹t
	//glRotatef(thetay, 1.0, 0.0, 0.0);
	//	Axes();


	glColor3f(1.0f, 1.0f, 1.0f); // Ustawienie koloru rysowania na bia³y
								 //glutWireTeapot(3.0);

	Drawing();

	glFlush();
	// Przekazanie poleceñ rysuj¹cych do wykonania





	glutSwapBuffers();
	//

}

/*************************************************************************************/

// Funkcja ustalaj¹ca stan renderowania



void MyInit(void)
{
	/*************************************************************************************/

	//  Definicja materia³u z jakiego zrobiony jest czajnik
	//  i definicja Ÿród³a œwiat³a

	/*************************************************************************************/

	// Zmienne dla obrazu tekstury



	GLbyte *pBytes;
	GLint ImWidth, ImHeight, ImComponents;
	GLenum ImFormat;
	/*************************************************************************************/
	// Definicja materia³u z jakiego zrobiony jest czajnik

	GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	// wspó³czynniki ka =[kar,kag,kab] dla œwiat³a otoczenia

	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	// wspó³czynniki kd =[kdr,kdg,kdb] œwiat³a rozproszonego

	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	// wspó³czynniki ks =[ksr,ksg,ksb] dla œwiat³a odbitego               

	GLfloat mat_shininess = { 100.0 };
	// wspó³czynnik n opisuj¹cy po³ysk powierzchni

	/*************************************************************************************/
	// Definicja Ÿród³a œwiat³a

	// po³o¿enie Ÿród³a


	GLfloat light_ambient0[] = { 0, 0, 0, 1.0 };
	GLfloat light_ambient1[] = { 0, 0, 0, 1.0 };
	// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a otoczenia
	// Ia = [Iar,Iag,Iab]

	GLfloat light_diffuse0[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_diffuse1[] = { 1.0, 1.0, 1.0, 1.0 };
	// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego
	// odbicie dyfuzyjne Id = [Idr,Idg,Idb]

	GLfloat light_specular0[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular1[] = { 0, 0, 1.0, 1.0 };
	// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego
	// odbicie kierunkowe Is = [Isr,Isg,Isb]

	GLfloat att_constant =  1.0;
	// sk³adowa sta³a ds dla modelu zmian oœwietlenia w funkcji
	// odleg³oœci od Ÿród³a

	GLfloat att_linear = 0.05;
	// sk³adowa liniowa dl dla modelu zmian oœwietlenia w funkcji
	// odleg³oœci od Ÿród³a

	GLfloat att_quadratic = 0.001;
	// sk³adowa kwadratowa dq dla modelu zmian oœwietlenia w funkcji
	// odleg³oœci od Ÿród³a

	/*************************************************************************************/
	// Ustawienie parametrów materia³u i Ÿród³a œwiat³a

	/*************************************************************************************/
	// Ustawienie patrametrów materia³u


	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

	/*************************************************************************************/
	// Ustawienie parametrów Ÿród³a

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);
	/*************************************************************************************/

	//glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient1);
	//glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
	//glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);
	//glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	//glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, att_constant);
	//glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, att_linear);
	//glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, att_quadratic);

	/*************************************************************************************/
	// Ustawienie opcji systemu oœwietlania sceny

	glShadeModel(GL_SMOOTH); // w³aczenie ³agodnego cieniowania
	glEnable(GL_LIGHTING);   // w³aczenie systemu oœwietlenia sceny
	glEnable(GL_LIGHT0);     // w³¹czenie Ÿród³a o numerze 0
	glEnable(GL_LIGHT1);
	glEnable(GL_DEPTH_TEST); // w³¹czenie mechanizmu z-bufora

							 /*************************************************************************************/
	/*************************************************************************************/

	// Teksturowanie bêdzie prowadzone tyko po jednej stronie œciany

	glEnable(GL_CULL_FACE);

	/*************************************************************************************/

	//  Przeczytanie obrazu tekstury z pliku o nazwie tekstura.tga

	pBytes = LoadTGAImage("tekstury\\D2_t.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);

	/*************************************************************************************/

	// Zdefiniowanie tekstury 2-D

	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);

	/*************************************************************************************/

	// Zwolnienie pamiêci

	free(pBytes);

	/*************************************************************************************/

	// W³¹czenie mechanizmu teksturowania

	glEnable(GL_TEXTURE_2D);

	/*************************************************************************************/

	// Ustalenie trybu teksturowania

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	/*************************************************************************************/

	// Okreœlenie sposobu nak³adania tekstur

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	// ..................................
	//       Pozosta³a czêœæ funkcji MyInit()

	// ..................................
	/*************************************************************************************/
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszc¹cy (wype³nienia okna) ustawiono na czarny
}

/*************************************************************************************/

// Funkcja ma za zadanie utrzymanie sta³ych proporcji rysowanych
// w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokoœæ i szerokoœæ okna) s¹
// przekazywane do funkcji za ka¿dym razem gdy zmieni siê rozmiar okna.



void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2angle = 360.0 / (float)horizontal;
	pix2angle1 = 360.0 / (float)vertical;  // przeliczenie pikseli na stopnie

	glMatrixMode(GL_PROJECTION);
	// Prze³¹czenie macierzy bie¿¹cej na macierz projekcji

	glLoadIdentity();
	// Czyszcznie macierzy bie¿¹cej
	GLfloat hor = glutGet(GLUT_WINDOW_WIDTH);
	GLfloat ver = glutGet(GLUT_WINDOW_HEIGHT);
	gluPerspective(60, 1, 1.0, 30.0);
	// Ustawienie parametrów dla rzutu perspektywicznego

	if (hor <= ver)
		glViewport((hor - ver) / 2, 0, ver, ver);

	else
		glViewport(0, (ver - hor) / 2, hor, hor);
	//glViewport(0, 0, horizontal, vertical);
	// Ustawienie wielkoœci okna okna widoku (viewport) w zale¿noœci
	// relacji pomiêdzy wysokoœci¹ i szerokoœci¹ okna

	glMatrixMode(GL_MODELVIEW);
	// Prze³¹czenie macierzy bie¿¹cej na macierz widoku modelu  

	glLoadIdentity();
	// Czyszczenie macierzy bie¿¹cej 

}

/*************************************************************************************/

// G³ówny punkt wejœcia programu. Program dzia³a w trybie konsoli



void main(void)
{
	srand(time(NULL));
	for (int i = 0; i < N; i++)
	{
		for (int k = 0; k < N; k++)
		{
			ColorArray[i][k].x = ((double)rand() / (RAND_MAX));
			ColorArray[i][k].y = ((double)rand() / (RAND_MAX));
			ColorArray[i][k].z = ((double)rand() / (RAND_MAX));
			//ColorArray[i][k].x = 1.0;
			//ColorArray[i][k].y = 1.0;
			//ColorArray[i][k].z = 1.0;
		}
	}


	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(300, 300);

	glutCreateWindow("Uk³ad wspó³rzêdnych 3-D");

	glutKeyboardFunc(keys);
	glutMouseFunc(Mouse);
	// Ustala funkcjê zwrotn¹ odpowiedzialn¹ za badanie stanu myszy

	glutMotionFunc(Motion);
	// Ustala funkcjê zwrotn¹ odpowiedzialn¹ za badanie ruchu myszy
	//glutIdleFunc(spinEgg);

	glutDisplayFunc(RenderScene);
	// Okreœlenie, ¿e funkcja RenderScene bêdzie funkcj¹ zwrotn¹
	// (callback function).  Bedzie ona wywo³ywana za ka¿dym razem
	// gdy zajdzie potrzba przeryswania okna 

	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcjê zwrotn¹ odpowiedzialn¹
	// zazmiany rozmiaru okna      

	MyInit();
	// Funkcja MyInit() (zdefiniowana powy¿ej) wykonuje wszelkie
	// inicjalizacje konieczne  przed przyst¹pieniem do renderowania 

	glEnable(GL_DEPTH_TEST);
	// W³¹czenie mechanizmu usuwania powierzchni niewidocznych

	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT

}

/*************************************************************************************/

