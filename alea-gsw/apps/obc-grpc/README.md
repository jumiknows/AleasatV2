


install and run
```
pipenv install
python -m obc_grpc
```

Generate python from proto file, run at apps/obc-grpc/obc_grpc
```
python -m grpc_tools.protoc --proto_path=. obc.proto --python_out=./alea/obc_grpc/generated --grpc_python_out=./alea/obc_grpc/generated --pyi_out=./alea/obc_grpc/generated
```