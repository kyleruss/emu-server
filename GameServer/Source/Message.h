#pragma once

struct MessageBody
{
	BYTE Category;
	BYTE Index;
	char Message[256];
};

#define MSGGET(x, y) ( (x) * 256 + (y)  )

class Message
{
public:
	Message();
	virtual ~Message();

	void Init();
	void Load();
	void Read(LPSTR File);

	LPSTR Get(int MessageCode);

private:
	std::vector<MessageBody> m_MessageList;

}; extern Message lMsg;

