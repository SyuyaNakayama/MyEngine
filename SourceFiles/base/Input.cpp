#include "Input.h"
#include "D3D12Common.h"
#include <cmath>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

Microsoft::WRL::ComPtr<IDirectInput8> Input::directInput;

//�Q�[���p�b�h�f�o�C�X�̍쐬-�f�o�C�X�񋓂̌��ʂ��󂯎��\����
struct DeviceEnumParameter
{
	LPDIRECTINPUTDEVICE8* gamePadDevice;
	int findCount;
};

Input* Input::GetInstance()
{
	static Input input;
	return &input;
}

void Input::Initialize()
{
	Result result;
	WindowsAPI* wAPI = WindowsAPI::GetInstance();

	result = DirectInput8Create(wAPI->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	// �L�[�{�[�h
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	result = keyboard->SetCooperativeLevel(wAPI->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	// �}�E�X
	result = directInput->CreateDevice(GUID_SysMouse, &mouse, NULL);
	result = mouse->SetDataFormat(&c_dfDIMouse2);
	result = mouse->SetCooperativeLevel(wAPI->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
#pragma region �Q�[���p�b�h
	DeviceEnumParameter parameter{};

	parameter.findCount = 0;
	parameter.gamePadDevice = &joystick;

	// GAMEPAD�𒲂ׂ�
	directInput->EnumDevices(
		DI8DEVTYPE_GAMEPAD,			// ��������f�o�C�X�̎��
		DeviceFindCallBack,			// �������Ɏ��s����֐�
		&parameter,					// �֐��ɓn���l
		DIEDFL_ATTACHEDONLY			// �������@
	);

	// JOYSTICK�𒲂ׂ�
	directInput->EnumDevices(
		DI8DEVTYPE_JOYSTICK,
		DeviceFindCallBack,
		&parameter,
		DIEDFL_ATTACHEDONLY
	);

	// �ǂ���������邱�Ƃ��o���Ȃ������玸�s
	if (parameter.findCount == 0) { return; }

	// ����J�n
	StartGamePadControl();
#pragma endregion
}

bool SetUpGamePadProperty(LPDIRECTINPUTDEVICE8 device)
{
	// �����[�h���Βl���[�h�Ƃ��Đݒ�
	DIPROPDWORD diprop;
	ZeroMemory(&diprop, sizeof(diprop));
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.diph.dwObj = 0;
	diprop.dwData = DIPROPAXISMODE_ABS;
	if (FAILED(device->SetProperty(DIPROP_AXISMODE, &diprop.diph))) { return false; }

	// X���̒l�͈̔͐ݒ�
	DIPROPRANGE diprg;
	ZeroMemory(&diprg, sizeof(diprg));
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwHow = DIPH_BYOFFSET;
	diprg.diph.dwObj = DIJOFS_X;
	diprg.lMin = -1000;
	diprg.lMax = 1000;
	if (FAILED(device->SetProperty(DIPROP_RANGE, &diprg.diph))) { return false; }

	// Y���̒l�͈̔͐ݒ�
	diprg.diph.dwObj = DIJOFS_Y;
	if (FAILED(device->SetProperty(DIPROP_RANGE, &diprg.diph))) { return false; }

	// RX���̒l�͈̔͐ݒ�
	diprg.diph.dwObj = DIJOFS_RX;
	if (FAILED(device->SetProperty(DIPROP_RANGE, &diprg.diph))) { return false; }
	// RY���̒l�͈̔͐ݒ�
	diprg.diph.dwObj = DIJOFS_RY;
	if (FAILED(device->SetProperty(DIPROP_RANGE, &diprg.diph))) { return false; }

	return true;
}

int CALLBACK Input::DeviceFindCallBack(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	DeviceEnumParameter* parameter = (DeviceEnumParameter*)pvRef;
	IDirectInputDevice8* device = nullptr;

	// ���ɔ������Ă���Ȃ�I��
	if (parameter->findCount >= 1) { return DIENUM_STOP; }

	// �f�o�C�X����
	HRESULT hr = directInput->CreateDevice(
		lpddi->guidInstance,
		parameter->gamePadDevice,
		NULL);

	if (FAILED(hr)) { return DIENUM_STOP; }

	// ���̓t�H�[�}�b�g�̎w��
	device = *parameter->gamePadDevice;
	hr = device->SetDataFormat(&c_dfDIJoystick);

	if (FAILED(hr)) { return DIENUM_STOP; }

	// �v���p�e�B�̐ݒ�
	if (!SetUpGamePadProperty(device)) { return DIENUM_STOP; }

	// �������x���̐ݒ�
	WindowsAPI* wAPI = WindowsAPI::GetInstance();
	device->SetCooperativeLevel(wAPI->GetHwnd(), DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	// ���������J�E���g
	parameter->findCount++;

	return DIENUM_CONTINUE;
}

void Input::StartGamePadControl()
{
	// �f�o�C�X����������ĂȂ�
	if (!joystick) { return; }

	// ����J�n
	Result result;
	result = joystick->Acquire();

	DIDEVCAPS cap;
	joystick->GetCapabilities(&cap);
	// �|�[�����O����
	if (cap.dwFlags & DIDC_POLLEDDATAFORMAT)
	{
		DWORD error = GetLastError();
		// �|�[�����O�J�n
		result = joystick->Poll();
	}
}

void Input::Update()
{
	keyboard->Acquire();
	oldkey = key;
	keyboard->GetDeviceState((DWORD)key.size(), (LPVOID)key.data());

	mouse->Acquire();
	mouseStatePre = mouseState;
	mouse->GetDeviceState(sizeof(mouseState), &mouseState);

	if (!joystick) { return; }
	joyStatePre = joyState;
	joystick->GetDeviceState(sizeof(joyState), &joyState);
}

Input::PadState Input::GetPadState()
{
	float angle = joyState.rgdwPOV[0] * PI / 18000.0f;
	Vector2 dirKey;
	if (joyState.rgdwPOV[0] != -1) { dirKey = { std::sin(angle), std::cos(angle) }; }
	return PadState(joyState.lX, joyState.lY, joyState.lRx, joyState.lRy, joyState.lZ, dirKey);
}

Vector2 Input::ConLStick(const float spd)
{
	float unresponsiveRange = 200;
	Vector2 vec;
	// X���ɂ���
	if (GetPadState().lX > unresponsiveRange) { vec.x = (float)GetPadState().lX / 1000 * spd; }
	else if (GetPadState().lX < -unresponsiveRange) { vec.x = (float)GetPadState().lX / 1000 * spd; }
	else { vec.x = 0.0f; }
	// Y���ɂ���
	if (GetPadState().lY > unresponsiveRange) { vec.y = -(float)GetPadState().lY / 1000 * spd; }
	else if (GetPadState().lY < -unresponsiveRange) { vec.y = -(float)GetPadState().lY / 1000 * spd; }
	else { vec.y = 0.0f; }

	return vec;
}

Vector2 Input::ConRStick(const float spd)
{
	float unresponsiveRange = 200;
	Vector2 vec;
	// X���ɂ���
	if (GetPadState().rX > unresponsiveRange) { vec.x = (float)GetPadState().rX / 1000 * spd; }
	else if (GetPadState().rX < -unresponsiveRange) { vec.x = (float)GetPadState().rX / 1000 * spd; }
	else { vec.x = 0.0f; }
	// Y���ɂ���
	if (GetPadState().rY > unresponsiveRange) { vec.y = -(float)GetPadState().rY / 1000 * spd; }
	else if (GetPadState().rY < -unresponsiveRange) { vec.y = -(float)GetPadState().rY / 1000 * spd; }
	else { vec.y = 0.0f; }

	return vec;
}

void Input::PadState::Normalize()
{
}
