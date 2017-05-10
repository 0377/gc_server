//
#ifndef __GAME_CONFIG_H__
#define __GAME_CONFIG_H__

#include "TSingleton.h"
#include "Size.h"
#include "MovePoint.h"
#include <string>
#include <map>
#include <vector>
#include <list>
#include "VisualCompent.h"
#include "XMLDecrypt.h"

//����������
enum SpecialFishType
{
	ESFT_NORMAL = 0,                        //��ͨ
	ESFT_KING,                              //����
	ESFT_KINGANDQUAN,                       //���
	ESFT_SANYUAN,                           //����Ԫ
	ESFT_SIXI,                              //����ϲ
	ESFT_MAX,                               //���
};

struct Effect
{
	int					nTypeID;            //Ч��ID
	std::vector<int>	nParam;             //����
};

struct Buffer
{
	int					nTypeID;            //ID
	float				fParam;             //����ֵ
	float				fLife;              //����ʱ��
};

//���� ǰ�� �Ӿ�
struct Visual
{
	int							nID;
	int							nTypeID;
	std::list<ImageInfo>		ImageInfoLive;
	std::list<ImageInfo>		ImageInfoDead;
};
//���� ����
struct BB
{
	float			fRadio;             //�뾶
	int				nOffestX;           //X����ƫ��
	int				nOffestY;           //Y����ƫ��
};
//����
struct BBX
{
	int				nID;                // ID
	std::list<BB>	BBList;             // �����б�
};
//��
struct Fish
{
	int					nTypeID;
	TCHAR				szName[256];			//����
	bool				bBroadCast;				//�Ƿ�㲥
	float				fProbability;			//����
	int					nVisualID;				//�Ӿ�ID
	int					nSpeed;					//�ٶ�
	int					nBoundBox;				//�󶨺���
	std::list<Effect>	EffectSet;				//Ч���б�
	std::list<Buffer>	BufferSet;				//buffer�б�
	bool				bShowBingo;				//��ʾЧ��
	std::string			szParticle;				//Ʒ��
	bool				bShakeScree;			//������  ����
	int					nLockLevel;				//�����ȼ�
};

struct Bullet
{
	int						nMulriple;				//�ӵ��۸�
	int						nSpeed;					//�ٶ�
	int						nMaxCatch;				//���ץ��ֵ�� �޵��� ����
	int						nBulletSize;			//�ӵ���С
	int						nCatchRadio;			//ץ�� �㲥���뾶��
	int						nCannonType;			//�ӵ�����
	std::map<int, float>	ProbabilitySet;			//���ʼ���
};
//ˢ������
enum RefershType
{
	ERT_NORMAL = 0,
	ERT_GROUP,					//��Ⱥ
	ERT_LINE,					//���
	ERT_SNAK,					//����
};
//������Ⱥ��
struct DistrubFishSet
{
	float				ftime;              //ʱ��
	int					nMinCount;          //��С����
	int					nMaxCount;          //�������
	int					nRefershType;       //ˢ�µ�����
	std::vector<int>	FishID;             //��ID�б�
	std::vector<int>	Weight;             //Ȩ��
	float				OffestX;            //X����ƫ��
    float				OffestY;            //Y����ƫ��
	float				OffestTime;         //ʱ��ƫ��
};
//ˢ��ʱ������
struct TroopSet
{
	float				fBeginTime;         //��ʼʱ��
	float				fEndTime;           //����ʱ��
	int					nTroopID;           //��ȺID
};
//��������
struct SceneSet
{
	int							nID;                //��ǰ����ID
	int							nNextID;            //��һ����ID
	std::string					szMap;              //�������֣� ����
	float						fSceneTime;         //����ʱ��
	std::list<TroopSet>			TroopList;          //ˢ��ʱ���б�
	std::list<DistrubFishSet>	DistrubList;        //�������б� 
};

struct SoundSet
{
	std::string	szFoundName;			//����
	int			m_nProbility;			//����
};

//��������
struct SpecialSet
{
	int			nTypeID;
	int			nSpecialType;			//����
	float		fProbability;			//����
	int			nMaxScore;				//������
	float		fCatchProbability;		//��׽����
	float		fVisualScale;			//�Ӿ��߶�
	int			nVisualID;				//�Ӿ�ID
	int			nBoundingBox;			//�󶨺���
	int			nLockLevel;				//�����ȼ�
};
//��һ�ο���
struct FirstFire
{
	int						nLevel;                 //�ȼ�
	int						nCount;                 //����
	int						nPriceCount;            //�۸�����
	std::vector<int>		FishTypeVector;         //������
	std::vector<int>		WeightVector;           //Ȩ��
};
//��Դ����  ǰ����Դ ����
enum ResourceType
{
	ERST_Sprite = 0,               //
	ERST_Animation,                //
	ERST_Particle,                 //
};
//��Ⱦ ����
enum RenderState
{
	ERSS_Normal = 1,
	ERSS_FIRE	= 2,
	ERSS_Mul	= 4,
	ERSS_Score	= 8,
};
//��λ����
enum PartType
{
	EPT_BASE = 0,
	EPT_CANNON,
	EPT_EFFECT,
	EPT_CANNUM,
	EPT_SCORE,
	EPT_TAG,
};
//����
struct CannonPart
{
    std::string		szResourceName;         //����
    int				nResType;               //��Դ����
    int				nType;                  //����
    MyPoint			Pos;                    //����
    int				FireOfffest;            //����ƫ�ƣ�
    float			RoateSpeed;             //�ӵ� �ٶ� ��
};

//�������� ����
struct CannonLock
{
	std::string		szLockIcon;             
	std::string		szLockLine;
	std::string		szLockFlag;
	MyPoint			Pos;
};
//����
struct CannonIon
{
	std::string		szIonFlag;
	MyPoint			Pos;
};

//�����ӵ�
typedef struct CannonBullet
{
	std::string		szResourceName;         //����
	int				nResType;               //��Դ����
	MyPoint			Pos;                    //����
	float			fScale;                 //��ģ
} CannonNet;

//���ڼ�
struct CannonSet
{
	int							nTypeID;             //����ID
	std::vector<CannonPart>		vCannonParts;        //��������
	std::vector<CannonBullet>	BulletSet;           //�ӵ���
	std::vector<CannonNet>		NetSet;              //����
};	

//�������ü�
struct CannonSetS
{
	int							nID;                //
	int							nNormalID;          //��ͨID
	int							nIonID;             //ͼ��ID
	int							nDoubleID;          //˫��ID
	bool						bRebound;           //�Ƿ��
	std::map<int, CannonSet>	Sets;               //��
};

class CGameConfig : public Singleton <CGameConfig>
{
public:	
	bool LoadSystemConfig(std::string szXmlFile, CXMLDecrypt* pcd = NULL);

	bool LoadFish(std::string szXmlFile, CXMLDecrypt* pcd = NULL);

	bool LoadVisual(std::string szXmlFile, CXMLDecrypt* pcd = NULL);
	
	bool LoadCannonSet(std::string szXmlFile, CXMLDecrypt* pcd = NULL);

	bool LoadBulletSet(std::string szXmlFile, CXMLDecrypt* pcd = NULL);
	bool LoadBoundBox(std::string szXmlFile, CXMLDecrypt* pcd = NULL);

	bool LoadScenes(std::string szXmlFile, CXMLDecrypt* pcd = NULL);

	bool LoadFishSound(std::string szXmlFile, CXMLDecrypt* pcd = NULL);

	bool LoadSpecialFish(std::string szXmlFile, CXMLDecrypt* pcd = NULL);

protected:

	CGameConfig();

	virtual ~CGameConfig();

	FriendBaseSingleton(CGameConfig);

public:
	int							nDefaultWidth;                      //Ĭ�Ͽ��
	int							nDefaultHeight;                     //Ĭ�ϸ߶�
	int							nWidth;                             //���
	int							nHeight;                            //�߶�
    //��ǰ�������Ϸ������ҵĶһ�����Ϊ%d��Ϸ�Ҷһ�%d��� nChangeRatioUserScore nChangeRatioFishScore
	int							nChangeRatioUserScore;              //��Ϸ�Ҷһ�����
	int							nChangeRatioFishScore;              //��Ҷһ�����

	int							nExchangeOnce;                      //�ı�һ�Σ�
	int							nFireInterval;                      //��������
	int							nMaxInterval;                       //�������
	int							nMinInterval;                       //��С����
	int							nMinNotice;                         //��С֪ͨ
	float						fAndroidProbMul;                    //��׿���ʣ�
	int							nPlayerCount;                       //�����
	int							nSpecialProb[ESFT_MAX];             //������ˢ�¸���

	std::map<int, Visual>		VisualMap;                          //�Ӿ��� ����
	std::map<int, Fish>			FishMap;                            //���
	std::vector<Bullet>			BulletVector;                       //�ӵ�����
	std::map<int, BBX>			BBXMap;                             //�󶨱�

	int							nAddMulBegin;                       //
	int							nAddMulCur;

	int							m_MaxCannon;                        //�����

	bool						bImitationRealPlayer;               //ģ����ң�

	std::vector<FirstFire>		FirstFireList;                      //��һ�ο����б�

	float						fHScale;                            //��
	float						fVScale;                            // ����

	std::map<int, SceneSet>		SceneSets;                          //������
	std::map<int, SoundSet>		FishSound;                          //������  ����
                                                                    
	std::map<int, SpecialSet>	KingFishMap;                        //������
	std::map<int, SpecialSet>	SanYuanFishMap;                     //����Ԫ�����
	std::map<int, SpecialSet>	SiXiFishMap;                        //��ϲ�����
                                                                    
	std::vector<CMovePoint>		CannonPos;                          //��������
                                                                    
	std::vector<CannonSetS>		CannonSetArray;                     //���ڼ�
                                                                    
	std::string					szCannonEffect;                     // ��Ч��  ����
	MyPoint						EffectPos;                          // Ч�����ꣿ ����
	int							nJettonCount;                       // �������� ����
	MyPoint						JettonPos;                          // �������� ����
	CannonLock					LockInfo;                           // �������� ����
                                                                    
	bool						ShowDebugInfo;                      // ����
	int							nShowGoldMinMul;                    //��ʾ��Ǯλ���� ����
	bool						ShowShadow;                         //��ʾ��Ӱ ����
                                                                    
	int							nIonMultiply;                       //�������ӣ�
	int							nIonProbability;	                //���Ӹ��ʣ�
	float						fDoubleTime;                        //˫��ʱ�䣿
                                                                    
	int							nMaxBullet;                         //����ӵ�
	int							nMaxSpecailCount;                   //���������
                                                                    
	float						fGiveRealPlayTime;                  // ����
	float						fGiveTime;                          // ����                         
	std::vector<int>			vGiveFish;                          // ����
	std::vector<int>			vGiveProb;                          // ����
                                                                    
	int							nSnakeHeadType;                     //��ͷ����
	int							nSnakeTailType;                     //��β����
};

#endif

