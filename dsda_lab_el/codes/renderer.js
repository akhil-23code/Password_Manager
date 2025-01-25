const passwordTable = {};
const siteNames = [];

// Handle Add Password button
document.getElementById('addPasswordButton').addEventListener('click', () => {
    const site = document.getElementById('site').value;
    const username = document.getElementById('username').value;
    const password = document.getElementById('password').value;

    if (!site || !username || !password) {
        alert('Please fill out all fields');
        return;
    }

    passwordTable[site] = { username, password };
    siteNames.push(site);

    alert('Password added successfully');
});

// Handle Retrieve Password button
document.getElementById('retrievePasswordButton').addEventListener('click', () => {
    const site = document.getElementById('retrieveSite').value;

    if (passwordTable[site]) {
        document.getElementById('retrievedPassword').textContent = `Password for ${site}: ${passwordTable[site].password}`;
    } else {
        alert('Password not found');
    }
});

// Handle Update Password button
document.getElementById('updatePasswordButton').addEventListener('click', () => {
    const site = document.getElementById('updateSite').value;
    const newPassword = document.getElementById('newPassword').value;

    if (passwordTable[site]) {
        passwordTable[site].password = newPassword;
        alert('Password updated successfully');
    } else {
        alert('Password not found');
    }
});

// Handle Delete Password button
document.getElementById('deletePasswordButton').addEventListener('click', () => {
    const site = document.getElementById('deleteSite').value;

    if (passwordTable[site]) {
        delete passwordTable[site];
        const index = siteNames.indexOf(site);
        if (index !== -1) siteNames.splice(index, 1);
        alert('Password deleted successfully');
    } else {
        alert('Password not found');
    }
});

// Handle Show List button
document.getElementById('showListButton').addEventListener('click', () => {
    const list = document.getElementById('passwordList');
    list.innerHTML = ''; // Clear the current list

    if (siteNames.length > 0) {
        siteNames.forEach(site => {
            const li = document.createElement('li');
            li.textContent = site;
            list.appendChild(li);
        });
    } else {
        alert('No passwords stored');
    }
});
