#include "Pipeline.h"
void Pipeline::Initialize(ID3D12Device* device, int blendMode)
{
	//�V�F�[�_�[
	VSCompile();
	PSCompile();

	//���_���C�A�E�g
	CreateVerLayout();

	// �O���t�B�b�N�X�p�C�v���C���ݒ�
	SetShader();
	SetRasterizer();
	SetBlendState(blendMode);
	SetVerLayout();
	SetTopology();
}

void Pipeline::Update()
{
	//commandList->DrawInstanced(vertices.size(), 1, 0, 0); // �S�Ă̒��_���g���ĕ`��
}

void Pipeline::VSCompile()
{
	HRESULT result;
	result = D3DCompileFromFile(
		L"BasicVS.hlsl", // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "vs_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&vsBlob, &errorBlob);

	// �G���[�Ȃ�
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}
}

void Pipeline::PSCompile()
{
	HRESULT result;
	result = D3DCompileFromFile(
		L"BasicPS.hlsl", // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "ps_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&psBlob, &errorBlob);

	// �G���[�Ȃ�
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}
}

void Pipeline::CreateVerLayout()
{
	inputLayout.push_back(
		{
		"POSITION",										//�Z�}���e�B�b�N��
		0,												//�����̃Z�}���e�B�b�N������Ƃ��g���C���f�b�N�X
		DXGI_FORMAT_R32G32B32_FLOAT,					//�v�f���ƃr�b�g����\��
		0,												//���̓X���b�g�C���f�b�N�X
		D3D12_APPEND_ALIGNED_ELEMENT,					//�f�[�^�̃I�t�Z�b�g�n(���͎̂����ݒ�)
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,		//���̓f�[�^���
		0												//��x�ɕ`�悷��C���X�^���X��(0�ł悢)
		}// (1�s�ŏ������ق������₷��)
		//���W�ȊO�ɐF�A�e�N�X�`��UV�Ȃǂ�n���ꍇ�͂���ɑ�����
	);
}

void Pipeline::SetShader()
{
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	// �T���v���}�X�N�̐ݒ�
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�
}

void Pipeline::SetRasterizer()
{
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;	// �J�����O���Ȃ�
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;	// �|���S�����h��Ԃ�(D3D12_FILL_MODE_WIREFRAME�ɂ���ƃ��C���[�t���[����)
	pipelineDesc.RasterizerState.DepthClipEnable = true;			// �[�x�N���b�s���O��L����
}

void Pipeline::SetBlendState(int blendMode)
{
	pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask
		= D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA�S�Ẵ`�����l����`��

	//�����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	//RBGA�S�Ẵ`�����l����`��

	if (blendMode == noBlend)
	{
		blenddesc.BlendEnable = false;				//�u�����h�𖳌��ɂ���
	}
	else
	{
		blenddesc.BlendEnable = true;				//�u�����h��L���ɂ���
	}
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//���Z
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//�\�[�X�̒l��100%�g��
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;	//�f�X�g�̒l��0%�g��

	//�u�����h���[�h�̕ύX
	if (blendMode == add)
	{
		//���Z����
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;		//���Z
		blenddesc.SrcBlend = D3D12_BLEND_ONE;		//�\�[�X�̒l��100%�g��
		blenddesc.DestBlend = D3D12_BLEND_ONE;		//�f�X�g�̒l��100%�g��
	}
	else if (blendMode == sub)
	{
		//���Z����
		blenddesc.BlendOp = D3D12_BLEND_OP_SUBTRACT;	//���Z
		blenddesc.SrcBlend = D3D12_BLEND_ONE;			//�\�[�X�̃A���t�@�l
		blenddesc.DestBlend = D3D12_BLEND_ONE;			//1.0f-�\�[�X�̃A���t�@�l
	}
	else if (blendMode == invers)
	{
		//�F���]
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;				//���Z
		blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;	//1.0f-�\�[�X�̃A���t�@�l
		blenddesc.DestBlend = D3D12_BLEND_ZERO;				//�g��Ȃ�
	}
	else if (blendMode == alpha)
	{
		//����������
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;				//���Z
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;			//�\�[�X�̃A���t�@�l
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;	//1.0f-�\�[�X�̃A���t�@�l
	}
}

void Pipeline::SetVerLayout()
{
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout.data();
	pipelineDesc.InputLayout.NumElements = inputLayout.size();
}

void Pipeline::SetTopology()
{
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// ���̑��̐ݒ�
	pipelineDesc.NumRenderTargets = 1; // �`��Ώۂ�1��
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255�w���RGBA
	pipelineDesc.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O
}
