#pragma once
template<typename T>
class LTRotation
{
public:
	T m_Quat[4];
	LTRotation()
	{
		m_Quat[0] = 0;
		m_Quat[1] = 0;
		m_Quat[2] = 0;
		m_Quat[3] = 1;
	}
	LTRotation(const LTRotation& q)
	{
		m_Quat[0] = q.m_Quat[0];
		m_Quat[1] = q.m_Quat[1];
		m_Quat[2] = q.m_Quat[2];
		m_Quat[3] = q.m_Quat[3];
	}
};
