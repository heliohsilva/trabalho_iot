import React, {useEffect, useState } from 'react';

const BoxGrid = () => {
    const [items, setItems] = useState([]);

    const API_URL = 'http://127.0.0.1:8000/estacoes';

    const fetchData = async () => {
        try {
            const res = await fetch(API_URL);
            const data = await res.json();
            setItems(data.slice(0, 12));

        }catch (error){
            console.error("api error: ", error);

        }
    };

    useEffect(() => {
        fetchData();

        const interval = setInterval(() => {
            fetchData();
        }, 1 * 20 * 1000);

        return () => clearInterval(interval);
    }, []);


    const handleEsvaziar = async (id, status) => {
        try {
            const response = await fetch(`http://0.0.0.0:8000/estacao/${id}`, {
            method: 'PUT',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({
                id: id,
                status: 'empty'
            }),
            });

            if (response.ok) {
                console.log('Successfully updated!');

            } else {
                console.error('Failed to update');
            }
        } catch (error) {
            console.error('Error:', error);
        }
        fetchData();
    };
    

    const handleDelete = async (id) => {
        try {
            const response = await fetch(`http://0.0.0.0:8000/estacao/${id}`, {
                method: 'DELETE', 
                headers: {
                    'Content-Type': 'application/json',
                }
            })
            if (response.ok) {
                console.log('Successfully updated!');

            } else {
                console.error('Failed to update');
            }
        }catch (error){
            console.error('Error: ', error);
        }

        fetchData();
    }


    return (
        <div className="p-8">  
            <h1 className="text-4xl font-bold mb-4">Estações</h1>
            <div className="grid grid-cols-1 sm:grid-cols-3 md:grid-cols-3 gap-8">
                {items.map(item => (
                    
                    <div key={item.id} className={`bg-white shadow rounded-xl p-10 border-4 ${
                    item.status === 'full' ? 'border-red-500' : 'border-green-500'}`}>
                        <h2 className="text-lg font-semibold mb-2">{item.id}</h2>
                        <p className="text-gray-600">{item.status}</p>
                        <button className="mt-4 text-center bg-green-500 hover:bg-green-700 text-white font-bold py-2 px-5 rounded"
                                onClick={() => handleEsvaziar(item.id)}>
                                Esvaziar
                        </button>
                        <button className="mt-4 ml-2 text-center bg-red-500 hover:bg-red-700 text-white font-bold py-2 px-5 rounded"
                                onClick={() => handleDelete(item.id)}>
                                Deletar estação
                        </button>
                    </div>
                    
                ))}
            </div>
        </div>
    );
};

export default BoxGrid;
