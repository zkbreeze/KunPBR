#include <kvs/KeyPressEventListener>

class SnapKey : public kvs::KeyPressEventListener 
{
	void update( kvs::KeyEvent* event );
};