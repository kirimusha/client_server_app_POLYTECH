<h1 align="center">Client-Server Graph Application</h1>

<p align="center">
Клиент-серверное приложение для поиска пути в графе с поддержкой TCP и UDP соединений.
</p>

<br>

<div align="center">

<table>
<tr>
<td width="50%" valign="top">

<h3>О проекте</h3>

Приложение состоит из сервера и клиента.  
Клиент отправляет описание графа и начальную/конечную вершины, после чего сервер выполняет поиск пути между вершинами.

</td>

<td width="50%" valign="top">

<h3>Поддерживаемые режимы</h3>

<ul>
<li>TCP</li>
<li>UDP</li>
</ul>

</td>
</tr>
</table>

</div>

<br>

<h2>Запуск</h2>

1. Сначала запустите сервер (в одном терминале);
2. Затем запустите клиент (в другом терминале).

<h3>TCP режим</h3>

<p>Терминал 1 (сервер):</p>

```bash
cd ~/client_server_app/build
./server 8080
````

<p>Терминал 2 (клиент):</p>

```bash
cd ~/client_server_app/build
./client 127.0.0.1 tcp 8080
```

<h3>UDP режим</h3>

<p>Терминал 1 (сервер):</p>

```bash
./server 12345 udp
```

<p>Терминал 2 (клиент):</p>

```bash
./client 127.0.0.1 udp 12345
```

<h2>Пример работы</h2>

После запуска клиента необходимо ввести описание графа:

```text
A B, B C, C D, D E, E F, F G, A C, B D
```

Затем указать начальную и конечную вершины:

```text
A G
```

Программа найдет путь между вершинами.

<h2>Завершение работы</h2>

Для остановки сервера:

```bash
Ctrl + C
```

flowchart TD

subgraph group_client["Client input and requests"]
  node_client_main["Client entry<br/>[ClientMain.cpp]"]
  node_client["Client requests<br/>[Client.cpp]"]
end

subgraph group_transport["Network protocols"]
  node_udp_protocol["UDP packets<br/>[UDPProtocol.h]"]
  node_network{{"TCP / UDP network"}}
end

subgraph group_server["Server and graph search"]
  node_server_main["Server entry<br/>[ServerMain.cpp]"]
  node_server["Request handling<br/>[Server.cpp]"]
  node_graph["Graph model<br/>[Graph.cpp]"]
  node_dijkstra["Shortest-path search<br/>[Dijkstra.h]"]
end

subgraph group_shared["Shared utilities"]
  node_parser["Input parsing<br/>[InputParser.cpp]"]
  node_file_reader["File input<br/>[FileReader.cpp]"]
  node_validator["Input validation<br/>[Validator.cpp]"]
  node_protocol["Request encoding<br/>[Protocol.cpp]"]
  node_logger["Logging<br/>[Logger.cpp]"]
end

node_user(("User"))

node_user -->|"provides input"| node_client_main
node_client_main -->|"parses input"| node_parser
node_client_main -.->|"reads graph file"| node_file_reader
node_client_main -->|"validates input"| node_validator
node_client_main -->|"submits request"| node_client
node_client -->|"encodes request"| node_protocol
node_client -.->|"frames UDP"| node_udp_protocol
node_client -->|"sends request"| node_network
node_network -->|"delivers request"| node_server
node_server_main -->|"starts server"| node_server
node_server -->|"decodes request"| node_protocol
node_server -.->|"handles UDP packets"| node_udp_protocol
node_server -->|"builds graph"| node_graph
node_server -->|"searches path"| node_dijkstra
node_server -->|"encodes response"| node_protocol
node_server -->|"sends response"| node_network
node_client -->|"logs events"| node_logger
node_server -->|"logs events"| node_logger

click node_client_main "https://github.com/kirimusha/client_server_app_polytech/blob/master/client/ClientMain.cpp"
click node_parser "https://github.com/kirimusha/client_server_app_polytech/blob/master/utils/InputParser.cpp"
click node_file_reader "https://github.com/kirimusha/client_server_app_polytech/blob/master/utils/FileReader.cpp"
click node_validator "https://github.com/kirimusha/client_server_app_polytech/blob/master/utils/Validator.cpp"
click node_client "https://github.com/kirimusha/client_server_app_polytech/blob/master/client/Client.cpp"
click node_protocol "https://github.com/kirimusha/client_server_app_polytech/blob/master/common/Protocol.cpp"
click node_udp_protocol "https://github.com/kirimusha/client_server_app_polytech/blob/master/common/UDPProtocol.h"
click node_server_main "https://github.com/kirimusha/client_server_app_polytech/blob/master/server/ServerMain.cpp"
click node_server "https://github.com/kirimusha/client_server_app_polytech/blob/master/server/Server.cpp"
click node_graph "https://github.com/kirimusha/client_server_app_polytech/blob/master/common/Graph.cpp"
click node_dijkstra "https://github.com/kirimusha/client_server_app_polytech/blob/master/common/Dijkstra.h"
click node_logger "https://github.com/kirimusha/client_server_app_polytech/blob/master/utils/Logger.cpp"

classDef toneNeutral fill:#f8fafc,stroke:#334155,stroke-width:1.5px,color:#0f172a
classDef toneBlue fill:#dbeafe,stroke:#2563eb,stroke-width:1.5px,color:#172554
classDef toneAmber fill:#fef3c7,stroke:#d97706,stroke-width:1.5px,color:#78350f
classDef toneMint fill:#dcfce7,stroke:#16a34a,stroke-width:1.5px,color:#14532d
classDef toneRose fill:#ffe4e6,stroke:#e11d48,stroke-width:1.5px,color:#881337
classDef toneIndigo fill:#e0e7ff,stroke:#4f46e5,stroke-width:1.5px,color:#312e81
classDef toneTeal fill:#ccfbf1,stroke:#0f766e,stroke-width:1.5px,color:#134e4a
class node_client_main,node_client,node_user toneBlue
class node_udp_protocol,node_network toneAmber
class node_server_main,node_server,node_graph,node_dijkstra toneMint
class node_parser,node_file_reader,node_validator,node_protocol,node_logger toneRose

