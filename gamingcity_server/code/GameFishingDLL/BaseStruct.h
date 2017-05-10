#pragma once
typedef struct CS_stTimeSync
{

    CS_stTimeSync()
    {
        Clear();
    }
    ~CS_stTimeSync()
    {
        Clear();
    }
    CS_stTimeSync& operator=(CS_stTimeSync& other)
    {
        this->chair_id = other.chair_id;			//����ID
        this->client_tick = other.client_tick;            //���ʱ��
        return *this;
    }
    void Clear()
    {
        chair_id    = 0;			//����ID
        client_tick = 0;            //���ʱ��
    }
    int	chair_id;				//����ID
    int	client_tick;            //���ʱ��
};

typedef struct CS_stChangeCannon
{

    CS_stChangeCannon()
    {
        Clear();
    }
    ~CS_stChangeCannon()
    {
        Clear();
    }
    void Clear()
    {
        chair_id = 0;				//����ID
        add = 0;            //���ʱ��
    }
    CS_stChangeCannon& operator=(CS_stChangeCannon& other)
    {
        this->chair_id = other.chair_id;			//����ID
        this->add = other.add;            
        return *this;
    }
    int	chair_id;				//����ID
    int	add;                    //0����һ�� ��0����
};


typedef struct CS_stFire
{
    CS_stFire()
    {
        Clear();
    }
    ~CS_stFire()
    {
        Clear();
    }
    void Clear()
    {
        chair_id = 0;				//����ID
        direction = 0;
        fire_time = 0;
        client_id = 0;
    }
    CS_stFire& operator=(CS_stFire& other)
    {
        this->chair_id = other.chair_id;			//����ID
        this->direction = other.direction;
        this->fire_time = other.fire_time;
        this->client_id = other.client_id;
        return *this;
    }
    int	    chair_id;		//����ID
    float   direction;		//����
    int     fire_time;		//����ʱ��
    int     client_id;		//�ӵ�ID��
};


typedef struct CS_stLockFish
{
    CS_stLockFish()
    {
        Clear();
    }
    ~CS_stLockFish()
    {
        Clear();
    }
    void Clear()
    {
        chair_id = 0;				//����ID
        lock = 0;
    }
    CS_stLockFish& operator= (CS_stLockFish& other)
    {
        this->chair_id = other.chair_id;			//����ID
        this->lock = other.lock;
        return *this;
    }
    int	    chair_id;		//����ID
    int     lock;		    //0��ʾ����
};

typedef struct CS_stNetcast
{
    CS_stNetcast()
    {
        Clear();
    }
    ~CS_stNetcast()
    {
        Clear();
    }
    void Clear()
    {
        bullet_id = 0;		  //�ӵ�ID
        data = 0;             //��ʹ�� ����
        fish_id = 0;          //��ID
    }
    CS_stNetcast& operator= (CS_stNetcast& other)
    {
        this->bullet_id = other.bullet_id;			//����ID
        this->data = other.data;
        this->fish_id = other.fish_id;
        return *this;
    }
    int	bullet_id;		  //�ӵ�ID
    int	data;             //��ʹ�� ����
    int	fish_id;          //��ID
};


typedef struct CS_stChangeCannonSet
{
    CS_stChangeCannonSet()
    {
        Clear();
    }
    ~CS_stChangeCannonSet()
    {
        Clear();
    }
    void Clear()
    {
        chair_id = 0;		  
        add = 0;               
    }
    CS_stChangeCannonSet& operator= (CS_stChangeCannonSet& other)
    {
        this->chair_id = other.chair_id;			//����ID
        this->add = other.add;
        return *this;
    }
    int	    chair_id;		//����ID
    int     add;		    //����ֵ 0 �� 
};

typedef struct CS_stTreasureEnd
{
    CS_stTreasureEnd()
    {
        Clear();
    }
    ~CS_stTreasureEnd()
    {
        Clear();
    }
    void Clear()
    {
        chair_id = 0;
        score = 0;
    }
    CS_stTreasureEnd& operator= (CS_stTreasureEnd& other)
    {
        this->chair_id = other.chair_id;			//����ID
        this->score = other.score;
        return *this;
    }
    int	    chair_id;		//����ID
    int     score;		    //ֵ 
};


typedef struct SC_stSendFish
{
    SC_stSendFish()
    {
        Clear();
    }
    ~SC_stSendFish()
    {
        Clear();
    }
    void Clear()
    {
       fish_id = 0;		    
       type_id = 0;         
       path_id = 0;         
       create_tick = 0;     
       offest_x = 0;        
       offest_y = 0;        
       dir = 0;             
       delay = 0;           
       server_tick = 0;     
       fish_speed = 0;     
       fis_type = 0;       
       troop = 0;          
       refersh_id = 0;     
    }
    SC_stSendFish& operator= (SC_stSendFish& other)
    {
        this->fish_id = other.fish_id;
        this->type_id = other.type_id;
        this->path_id = other.path_id;
        this->create_tick = other.create_tick;
        this->offest_x = other.offest_x;
        this->offest_y = other.offest_y;
        this->dir = other.dir;
        this->delay = other.delay;
        this->server_tick = other.server_tick;
        this->fish_speed = other.fish_speed;
        this->fis_type = other.fis_type;
        this->troop = other.troop;
        this->refersh_id = other.refersh_id;
        return *this;
    }
    int	    fish_id;		 //��ID
    int	    type_id;         //���ͣ�
    int	    path_id;         //·��ID
    int	    create_tick;     //����ʱ��
    float	offest_x;        //X����
    float	offest_y;        //Y����
    float	dir;             //����
    float	delay;           //��ʱ
    int	    server_tick;     //ϵͳʱ��
    float	fish_speed;      //���ٶ�
    int	    fis_type;        //�����ͣ�
    int	    troop;           //�Ƿ���Ⱥ
    int	    refersh_id;      //��ȡˢ��ID��
};


typedef struct stLuaMsgType
{
    stLuaMsgType()
    {
        Clear();
    }
    ~stLuaMsgType()
    {
        Clear();
    }
    void Clear()
    {
        wValue = 0;
        bRet = false;
        cbByte = 0;
        lValue = 0;
        strValue = "";
    }
    stLuaMsgType& operator= (stLuaMsgType& other)
    {
        this->wValue = other.wValue;
        this->bRet = other.bRet;
        this->cbByte = other.cbByte;
        this->strValue = other.strValue;
        this->lValue = other.lValue;
        return *this;
    }
    WORD	    wValue;		 
    bool	    bRet   ;  
    BYTE        cbByte;
    LONGLONG    lValue;
    std::string strValue;
};

typedef struct stLuaMsg
{
    stLuaMsg()
    {
        Clear();
    }
    ~stLuaMsg()
    {
        Clear();
    }
    void Clear()
    {
        m_TableID = 0;
        m_pMsg = NULL;
        m_iMsgID = NULL;
        m_iGuID = 0;
    }
    int     m_iGuID;         //�������ID
    int	    m_TableID;		 //����ID
    int     m_iMsgID;        //��ϢID
    void*	m_pMsg;          //��Ϣ��ָ��
};
