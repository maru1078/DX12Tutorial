#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <Windows.h>
#include <memory>
#include <vector>

class Dx12;
class SphereRenderer;
class PeraPolygon;
class LineDrawer;

class Application
{
public:

	Application(float windowWidth, float windowHeight);

public:

	void Initialize();
	void Run();
	void Finalize();

private:

	HWND m_hWnd;
	WNDCLASSEX m_windowClass;
	float m_windowWidth;
	float m_windowHeight;
	std::shared_ptr<Dx12> m_dx12;
	std::shared_ptr<SphereRenderer> m_sphereRenderer;
	std::shared_ptr<PeraPolygon> m_pera;
	std::shared_ptr<LineDrawer> m_lineDrawer;
};

#endif // !APPLICATION_H_

