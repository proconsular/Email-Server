import {controller, state, Views} from "./state.js";

export class LoginView {
    constructor() {
        this.submit = this.submit.bind(this)
    }

    async submit() {
        let state = {
            username: '',
            password: ''
        }
        let inputs = document.querySelectorAll("#view_login input");
        for (let input of inputs) {
            state[input.name] = input.value
        }
        let ok = await controller.login(state.username, state.password);
        if (ok) {
            controller.set_view(Views.Main);
        }
        controller.notify(ok ? "Login Succeeded" : "Login Failed")
    }

    init() {
        let view = state.loaded_views[Views.Login];
        let container = document.createElement("div");
        container.innerHTML = view;
        controller.set(container);
        let button = document.querySelector("#view_login button");
        if (button) {
            button.addEventListener("click", this.submit);
        }
    }
}