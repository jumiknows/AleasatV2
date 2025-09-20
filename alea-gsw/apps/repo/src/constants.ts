const aleaSwRepoDeployTokenUser =
  process.env.ALEA_SW_REPO_DEPLOY_TOKEN_USER ?? "gitlab+deploy-token-6547986";

const aleaSwRepoDeployTokenPassword =
  process.env.ALEA_SW_REPO_DEPLOY_TOKEN_PASSWORD ?? "gldt--4U316U6PC1LhSfxHmsr";

if (!aleaSwRepoDeployTokenUser || !aleaSwRepoDeployTokenPassword) {
  throw new Error(
    "ALEA_SW_REPO_DEPLOY_TOKEN_USER or ALEA_SW_REPO_DEPLOY_TOKEN_PASSWORD is not set",
  );
}

export default {
  aleaSwRepoDeployToken: {
    user: aleaSwRepoDeployTokenUser,
    password: aleaSwRepoDeployTokenPassword,
  },
};
