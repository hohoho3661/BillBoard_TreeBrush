#include "Framework.h"
#include "Freedom.h"

Freedom::Freedom()
	: move(20.0f), rotation(2.0f)
{
}

Freedom::~Freedom()
{
}

void Freedom::Update()
{
	if (Mouse::Get()->Press(1))
	{
		D3DXVECTOR3& f = Forward();
		D3DXVECTOR3& r = Right();
		D3DXVECTOR3& u = Up();

		D3DXVECTOR3 position;
		Position(&position);

		if (Keyboard::Get()->Press('W'))
			position += f * move * Time::Delta();
		else if (Keyboard::Get()->Press('S'))
			position += -f * move * Time::Delta();

		if (Keyboard::Get()->Press('D'))
			position += r * move * Time::Delta();
		else if (Keyboard::Get()->Press('A'))
			position += -r * move * Time::Delta();

		if (Keyboard::Get()->Press('E'))
			position += u * move * Time::Delta();
		else if (Keyboard::Get()->Press('Q'))
			position += -u * move * Time::Delta();

		Position(position);



		D3DXVECTOR2 R;
		Rotation(&R);

		D3DXVECTOR3 val = Mouse::Get()->GetMoveValue(); //무브벨류 = 마우스 이동한거리 차
		R.x += val.y *rotation * Time::Delta();
		R.y += val.x *rotation * Time::Delta();

		Rotation(R);
	}
}

void Freedom::Speed(float move, float rotation)
{
	this->move = move;
	this->rotation = rotation;

}
