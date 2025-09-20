const obcFwRepoDeployTokenUser =
  process.env.OBC_FW_REPO_DEPLOY_TOKEN_USER ?? "gitlab+deploy-token-5135809";

const obcFwRepoDeployTokenPassword =
  process.env.OBC_FW_REPO_DEPLOY_TOKEN_PASSWORD ?? "gldt-e-7NWykDxxTwqy9tSXHV";

if (!obcFwRepoDeployTokenUser || !obcFwRepoDeployTokenPassword) {
  throw new Error(
    "OBC_FW_REPO_DEPLOY_TOKEN_USER or OBC_FW_REPO_DEPLOY_TOKEN_PASSWORD is not set",
  );
}

export default {
  obcFwRepoDeployToken: {
    user: obcFwRepoDeployTokenUser,
    password: obcFwRepoDeployTokenPassword,
  },
};
