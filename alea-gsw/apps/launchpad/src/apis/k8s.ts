import k8s from "@kubernetes/client-node";

const kc = new k8s.KubeConfig();
kc.loadFromDefault();

let batchV1Api: k8s.BatchV1Api | undefined;
let coreV1Api: k8s.CoreV1Api | undefined;
let logsApi: k8s.LogsApi | undefined;
let currentNamespace: string | undefined;
let log: k8s.Log | undefined;

let k8sApi:
  | {
      batchV1Api: k8s.BatchV1Api;
      coreV1Api: k8s.CoreV1Api;
      logsApi: k8s.LogsApi;
      log: k8s.Log;
      currentNamespace: string;
    }
  | undefined;

try {
  batchV1Api = kc.makeApiClient(k8s.BatchV1Api);
  coreV1Api = kc.makeApiClient(k8s.CoreV1Api);
  logsApi = kc.makeApiClient(k8s.LogsApi);
  currentNamespace = kc.getContexts()[0].namespace!;
  log = new k8s.Log(kc);

  k8sApi = {
    batchV1Api,
    coreV1Api,
    logsApi,
    log,
    currentNamespace,
  };
} catch (error) {
  console.warn("Error while creating k8s API clients", error);
}

export default k8sApi;
