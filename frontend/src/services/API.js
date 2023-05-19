import axios from 'axios';

let baseURL;

// nodejs development
if (window.location.port == 5173) {
  // Use mDNS address
  baseURL = 'http://snek.local/api/v1';
} else {
  baseURL = '/api/v1';
}

const instance = axios.create({
  baseURL,
});

export default instance;
